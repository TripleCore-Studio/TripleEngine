#include <triple/game/GameLayer.h>

#include <triple/math/MathCommon.h>

#include <triple/log/Logger.h>

#include <triple/core/event/WindowResizeEvent.h>

#include "triple/game/utils/TransformUtils.h"

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/CameraComponent.h"

#include "triple/game/render/RenderSystem.h"

#include "triple/game/asset/Model.h"
#include "triple/game/asset/Texture.h"
#include "triple/game/asset/Shader.h"
#include "triple/game/asset/DefaultAssets.h"

#include "triple/game/asset/ModelLoader.h"
#include "triple/game/asset/TextureLoader.h"
#include "triple/game/asset/ShaderLoader.h"
#include "triple/game/asset/MaterialLoader.h"

namespace triple::game {
	void GameLayer::onAttach(const core::EngineContext &ctx) {
		m_context = ctx;
		load();
	}
	void GameLayer::onDetach() {}

	void GameLayer::onUpdate(float dt) {
		m_scene->onUpdate(dt);
		cameraUpdate(dt);
	}

	void GameLayer::onRender(float t) {
		entt::registry &registry = getActiveScene()->registry();

		auto &transform = registry.get<TransformComponent>(m_cameraEntity);
		auto &cameraComp = registry.get<CameraComponent>(m_cameraEntity);

		gfx::CameraData camera{
		    cameraComp.viewMatrix, cameraComp.projectionMatrix, transform.position
		};

		const uint32_t kWidth = static_cast<uint32_t>(m_context.window->getWidth());
		const uint32_t kHeight = static_cast<uint32_t>(m_context.window->getHeight());

		gfx::ViewDesc gbufferViewDesc{};
		gbufferViewDesc.camera = camera;
		gbufferViewDesc.target = m_gbufferTarget;
		gbufferViewDesc.viewport = {0, 0, kWidth, kHeight};
		gbufferViewDesc.clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
		gbufferViewDesc.clearDepth = true;
		gbufferViewDesc.activeColorAttachments = {0, 1, 2};
		gbufferViewDesc.clearColorAttachments = {0, 1, 2};
		m_context.renderer->updateView(m_gbufferView, gbufferViewDesc);

		gfx::ViewDesc transparentViewDesc{};
		transparentViewDesc.camera = camera;
		transparentViewDesc.target = m_gbufferTarget;
		transparentViewDesc.viewport = {0, 0, kWidth, kHeight};
		transparentViewDesc.clearDepth = false;
		transparentViewDesc.activeColorAttachments = {3};
		transparentViewDesc.clearColorAttachments = {};
		m_context.renderer->updateView(m_transparentView, transparentViewDesc);

		RenderSystem::submitScene(
		    registry, m_gbufferView, m_transparentView, *m_context.frameArena, transform.position
		);

		gfx::TextureHandle gAlbedo = m_context.renderer->getRenderTargetTexture(m_gbufferTarget, 0);
		gfx::TextureHandle gNormal = m_context.renderer->getRenderTargetTexture(m_gbufferTarget, 1);
		gfx::TextureHandle gMaterial =
		    m_context.renderer->getRenderTargetTexture(m_gbufferTarget, 2);

		RenderSystem::submitFullscreenPass(
		    m_lightingView,
		    gfx::RenderPass::Lighting,
		    kLightingPassShaderName,
		    {gAlbedo, gNormal, gMaterial}
		);

		gfx::TextureHandle gLitColor =
		    m_context.renderer->getRenderTargetTexture(m_gbufferTarget, 3);

		RenderSystem::submitFullscreenPass(
		    m_postProcessView, gfx::RenderPass::PostProcess, kPostProcessShaderName, {gLitColor}
		);
	}

	void GameLayer::onEvent(core::Event &e) {
		if (e.getTypeID() == core::WindowResizeEvent::staticTypeID()) {
			auto &size = static_cast<core::WindowResizeEvent &>(e);
			// minimized window reports 0x0, a zero-sized target would be incomplete
			if (size.getWidth() > 0 && size.getHeight() > 0) {
				createFramebuffers(
				    static_cast<uint32_t>(size.getWidth()), static_cast<uint32_t>(size.getHeight())
				);
			}
		}
	}

	void GameLayer::cameraUpdate(float dt) {
		entt::registry &registry = getActiveScene()->registry();

		TransformComponent &cameraTransform = registry.get<TransformComponent>(m_cameraEntity);

		math::Vec2 delta = m_context.inputSystem->getMouseDelta();
		cameraTransform.rotationEuler.y += -delta.x * cameraSettings.sensitivity; // yaw
		cameraTransform.rotationEuler.x += delta.y * cameraSettings.sensitivity;  // pitch
		cameraTransform.rotationEuler.x =
		    math::clamp(cameraTransform.rotationEuler.x, -89.0f, 89.0f);

		math::Vec3 dir(0, 0, 0);
		if (m_context.inputSystem->isKeyDown(core::KeyCode::W)) {
			dir.z += 1;
		}
		if (m_context.inputSystem->isKeyDown(core::KeyCode::S)) {
			dir.z -= 1;
		}
		if (m_context.inputSystem->isKeyDown(core::KeyCode::A)) {
			dir.x -= 1;
		}
		if (m_context.inputSystem->isKeyDown(core::KeyCode::D)) {
			dir.x += 1;
		}
		if (m_context.inputSystem->isKeyDown(core::KeyCode::Space)) {
			dir.y += 1;
		}
		if (m_context.inputSystem->isKeyDown(core::KeyCode::LeftShift)) {
			dir.y -= 1;
		}

		if (m_context.inputSystem->isMouseButtonDown(core::MouseButton::Button5)) {
			cameraSettings.cameraSpeed += cameraSettings.cameraSpeedChange * dt;
		} else if (m_context.inputSystem->isMouseButtonDown(core::MouseButton::Button4)) {
			cameraSettings.cameraSpeed -= cameraSettings.cameraSpeedChange * dt;
		}

		cameraSettings.cameraSpeed = math::clamp(
		    cameraSettings.cameraSpeed, cameraSettings.cameraSpeedMin, cameraSettings.cameraSpeedMax
		);

		if (dir.length() > 0) {
			dir = dir.normalized() * cameraSettings.cameraSpeed * dt;

			math::Vec3 fwd = TransformUtils::forward(cameraTransform);
			if (cameraSettings.lockY) {
				fwd.y = 0;
				fwd = fwd.normalized();
			}
			math::Vec3 rightVec = TransformUtils::right(cameraTransform);
			math::Vec3 up(0, 1, 0);

			cameraTransform.position += fwd * dir.z;
			cameraTransform.position += rightVec * dir.x;
			cameraTransform.position += up * dir.y;
		}
	}

	void GameLayer::cameraInit() {
		entt::registry &registry = getActiveScene()->registry();

		m_cameraEntity = registry.create();

		auto &transform = registry.emplace<TransformComponent>(m_cameraEntity);

		auto &camera = registry.emplace<CameraComponent>(m_cameraEntity);
		camera.aspectRatio = 16.0f / 9.0f;
		camera.fov = 70.0f;
		camera.farPlane = 1000.0f;
		camera.nearPlane = 0.01f;

		setActiveCamera(m_cameraEntity);
	}

	void GameLayer::load() {
		m_assetManager = std::make_unique<AssetManager>(m_context.bus);
		m_scene = std::make_unique<Scene>(m_assetManager.get());
		m_gpuRegistry = std::make_unique<GpuResourceRegistry>(
		    m_context.renderer, m_context.bus, m_assetManager.get()
		);

		m_assetManager->registerLoader<Model>(std::make_unique<ModelLoader>(m_assetManager.get()));
		m_assetManager->registerLoader<Texture>(std::make_unique<TextureLoader>());
		m_assetManager->registerLoader<Shader>(std::make_unique<ShaderLoader>());
		m_assetManager->registerLoader<Material>(
		    std::make_unique<MaterialLoader>(m_assetManager.get())
		);

		registerDefaultAssets();

		RenderSystem::setRegistry(m_gpuRegistry.get());
		RenderSystem::setRenderer(m_context.renderer);
		RenderSystem::setAssetManager(m_assetManager.get());

		configureRenderPipeline();

		cameraInit();
	}

	TypedAssetID<Texture> genSolidTexture(
	    AssetManager &assetManager,
	    const std::string &name,
	    uint8_t r,
	    uint8_t g,
	    uint8_t b,
	    uint8_t a
	) {
		Texture tex;
		tex.width = 1;
		tex.height = 1;
		tex.channels = 4;
		tex.pixels = {r, g, b, a};

		return assetManager.create<Texture>(name, std::move(tex));
	}

	void GameLayer::registerDefaultAssets() {
		TypedAssetID<Texture> ad = genSolidTexture(
		    *m_assetManager, std::string(kDefaultAlbedoTextureName), 255, 255, 255, 255
		);
		TypedAssetID<Texture> mtd = genSolidTexture(
		    *m_assetManager, std::string(kDefaultMetallicTextureName), 0, 0, 0, 255
		);
		TypedAssetID<Texture> nd = genSolidTexture(
		    *m_assetManager, std::string(kDefaultNormalTextureName), 128, 128, 255, 255
		);
		TypedAssetID<Texture> rd = genSolidTexture(
		    *m_assetManager, std::string(kDefaultRoughnessTextureName), 255, 255, 255, 255
		);

		std::string shaderPath = "assets/shaders/";

		std::optional<TypedAssetID<Shader>> sd = m_assetManager->load<Shader>(
		    std::string(kGBufferShaderName), ShaderLoadParams{shaderPath + "material/plastic"}
		);

		std::optional<TypedAssetID<Shader>> lp = m_assetManager->load<Shader>(
		    std::string(kLightingPassShaderName),
		    ShaderLoadParams{shaderPath + "standard/lighting_pass", false}
		);

		std::optional<TypedAssetID<Shader>> pp = m_assetManager->load<Shader>(
		    std::string(kPostProcessShaderName),
		    ShaderLoadParams{shaderPath + "standard/postprocess", false}
		);

		if (!ad.isValid() || !mtd.isValid() || !nd.isValid() || !rd.isValid() || !sd || !lp ||
		    !pp) {
			triple::log::Logger::moduleCritical(
			    "GameLayer",
			    "The default resources were not loaded properly, and the program "
			    "cannot continue working normally."
			);
			return; // TODO: currently just logs, should actually stop startup later
		}

		Material material;
		material.blendMode = MaterialBlendMode::Opaque;
		material.shader = *sd;
		material.params[kMetallicParam.data()] = {{0.1f}};
		material.params[kRoughnessParam.data()] = {{1.0f}};
		material.params[kAlbedoColorParam.data()] = {{1.0f, 1.0f, 1.0f, 1.0f}};

		material.textures[kAlbedoMapSlot.data()] = ad;
		material.textures[kMetallicMapSlot.data()] = mtd;
		material.textures[kNormalMapSlot.data()] = nd;
		material.textures[kRoughnessMapSlot.data()] = rd;

		m_assetManager->create<Material>(std::string(kDefaultMaterialName), material);
	}

	void GameLayer::configureRenderPipeline() {
		const uint32_t kWidth = static_cast<uint32_t>(m_context.window->getWidth());
		const uint32_t kHeight = static_cast<uint32_t>(m_context.window->getHeight());

		// clang-format off
		const float kQuadVertices[] = {
			// position       // uv
			-1.0f,  1.0f,     0.0f, 1.0f,
			-1.0f, -1.0f,     0.0f, 0.0f,
			1.0f, -1.0f,     1.0f, 0.0f,
			1.0f,  1.0f,     1.0f, 1.0f,
		};
		// clang-format on
		const uint32_t kQuadIndices[] = {0, 1, 2, 0, 2, 3};

		gfx::VertexAttributeDesc quadPos;
		quadPos.semantic = "aPosition";
		quadPos.type = gfx::VertexAttribType::Vec2;
		quadPos.location = 0;
		quadPos.offset = 0;

		gfx::VertexAttributeDesc quadUV;
		quadUV.semantic = "aUV";
		quadUV.type = gfx::VertexAttribType::Vec2;
		quadUV.location = 1;
		quadUV.offset = sizeof(float) * 2;

		gfx::VertexLayout quadLayout;
		quadLayout.attributes = {quadPos, quadUV};
		quadLayout.stride = sizeof(float) * 4;

		gfx::GeometryDesc quadDesc;
		quadDesc.vertexData = kQuadVertices;
		quadDesc.vertexCount = 4;
		quadDesc.vertexStride = quadLayout.stride;
		quadDesc.layout = quadLayout;
		quadDesc.indices = kQuadIndices;
		quadDesc.indexCount = 6;

		m_fullscreenQuad = m_context.renderer->uploadGeometry(quadDesc);
		RenderSystem::setFullscreenQuad(m_fullscreenQuad, 6);

		createFramebuffers(kWidth, kHeight);
	}

	// Everything whose size depends on the window: the gbuffer target and the views on top of it.
	// Called at startup and again on every resize.
	// TODO(hidpi): width/height come from the window size (glfwSetWindowSizeCallback, screen
	// coordinates), not the framebuffer size in pixels. On a HiDPI display (macOS Retina, or
	// Windows with GLFW_SCALE_TO_MONITOR) the two differ, and the picture will render into only a
	// part of the window (e.g. a quarter at 2x). Fix: use glfwGetFramebufferSize /
	// glfwSetFramebufferSizeCallback in GLWindow and feed that size into WindowResizeEvent and
	// window->getWidth()/getHeight().
	void GameLayer::createFramebuffers(uint32_t kWidth, uint32_t kHeight) {
		if (m_gbufferTarget.isValid()) {
			m_context.renderer->destroyView(m_gbufferView);
			m_context.renderer->destroyView(m_lightingView);
			m_context.renderer->destroyView(m_transparentView);
			m_context.renderer->destroyView(m_postProcessView);
			m_context.renderer->destroyRenderTarget(m_gbufferTarget);
		}

		// gbuffer render target (4 attachments: albedo/normal/material/litColor)
		gfx::RenderTargetDesc gbufferDesc;
		gbufferDesc.type = gfx::RenderTargetType::Texture;
		gbufferDesc.width = kWidth;
		gbufferDesc.height = kHeight;
		gbufferDesc.colorFormats = {
		    gfx::TextureFormat::Rgba8, // 0: gAlbedo
		    // TODO: temporary Rgba8 placeholder. Once gfx::TextureFormat is finalized,
		    // switch to RG16F (or similar) + octahedral normal encoding for better
		    // precision — packing world-space normals into 8-bit [0,1] is fine for
		    // the no-lighting pipeline test, but will show banding once real PBR
		    // lighting reads this buffer.
		    gfx::TextureFormat::Rgba8, // 1: gNormal
		    // TODO: temporary Rgba8 placeholder. Real layout should be decided once
		    // ComputePBR is written — likely R8 metallic + G8 roughness is enough,
		    // but leaving Rgba8 for now keeps all 3 gbuffer attachments uniform
		    // while we're only testing the pipeline routing, not real material data.
		    gfx::TextureFormat::Rgba8, // 2: gMaterial
		    gfx::TextureFormat::Rgba8, // 3: gLitColor
		};
		gbufferDesc.hasDepth = true;

		m_gbufferTarget = m_context.renderer->createRenderTarget(gbufferDesc);

		gfx::ViewDesc gbufferViewDesc{};
		gbufferViewDesc.target = m_gbufferTarget;
		gbufferViewDesc.viewport = {0, 0, kWidth, kHeight};
		gbufferViewDesc.clearColor = {0.2f, 0.2f, 0.2f, 1.0f};
		gbufferViewDesc.clearDepth = true;
		gbufferViewDesc.activeColorAttachments = {0, 1, 2};
		gbufferViewDesc.clearColorAttachments = {0, 1, 2};
		m_gbufferView = m_context.renderer->createView(gbufferViewDesc);

		gfx::ViewDesc lightingViewDesc{};
		lightingViewDesc.target = m_gbufferTarget;
		lightingViewDesc.viewport = {0, 0, kWidth, kHeight};
		lightingViewDesc.clearColor = {0.2f, 0.2f, 0.2f, 1.0f};
		lightingViewDesc.clearDepth = false;
		lightingViewDesc.activeColorAttachments = {3};
		lightingViewDesc.clearColorAttachments = {3};
		m_lightingView = m_context.renderer->createView(lightingViewDesc);

		gfx::ViewDesc transparentViewDesc{};
		transparentViewDesc.target = m_gbufferTarget;
		transparentViewDesc.viewport = {0, 0, kWidth, kHeight};
		transparentViewDesc.clearColor = {0.2f, 0.2f, 0.2f, 1.0f};
		transparentViewDesc.clearDepth = false;
		transparentViewDesc.activeColorAttachments = {3};
		transparentViewDesc.clearColorAttachments = {};
		m_transparentView = m_context.renderer->createView(transparentViewDesc);

		gfx::ViewDesc postProcessViewDesc{};
		postProcessViewDesc.target = m_context.renderer->getBackBufferTarget();
		postProcessViewDesc.viewport = {0, 0, kWidth, kHeight};
		postProcessViewDesc.clearColor = {0.2f, 0.2f, 0.2f, 1.0f};
		postProcessViewDesc.clearDepth = true;
		postProcessViewDesc.activeColorAttachments = {0};
		postProcessViewDesc.clearColorAttachments = {0};
		m_postProcessView = m_context.renderer->createView(postProcessViewDesc);

		m_context.renderer->setViewOrder(
		    {m_gbufferView, m_lightingView, m_transparentView, m_postProcessView}
		);
	}
} // namespace triple::game
