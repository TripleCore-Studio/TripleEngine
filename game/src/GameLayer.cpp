#include <triple/game/GameLayer.h>

#include <triple/math/MathCommon.h>

#include <triple/log/Logger.h>

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

namespace triple::game {
	void GameLayer::onAttach(const core::EngineContext &ctx) {
		m_renderer = ctx.renderer;
		m_inputSystem = ctx.inputSystem;
		m_bus = ctx.bus;
		load();
	}
	void GameLayer::onDetach() {}

	void GameLayer::onUpdate(float dt) {
		m_scene->onUpdate(dt);
		cameraUpdate(dt);
	}

	void GameLayer::onRender(float t) {
		entt::registry &registry = getActiveScene()->getRegistry();

		auto &transform = registry.get<TransformComponent>(m_cameraEntity);
		auto &cameraComp = registry.get<CameraComponent>(m_cameraEntity);

		gfx::FrameContext ctx;

		ctx.camera =
		    gfx::CameraData{cameraComp.viewMatrix, cameraComp.projectionMatrix, transform.position};

		RenderSystem::buildRenderCommands(registry, ctx.commands);
		ctx.time = t;

		if (m_onFrameContext)
			m_onFrameContext(ctx);

		m_renderer->RenderFrame(ctx);
	}

	void GameLayer::onEvent(core::Event &e) {}

	void GameLayer::cameraUpdate(float dt) {
		entt::registry &registry = getActiveScene()->getRegistry();

		TransformComponent &cameraTransform = registry.get<TransformComponent>(m_cameraEntity);

		math::Vec2 delta = m_inputSystem->getMouseDelta();
		cameraTransform.rotationEuler.y += -delta.x * cameraSettings.sensitivity; // yaw
		cameraTransform.rotationEuler.x += delta.y * cameraSettings.sensitivity;  // pitch
		cameraTransform.rotationEuler.x =
		    math::clamp(cameraTransform.rotationEuler.x, -89.0f, 89.0f);

		math::Vec3 dir(0, 0, 0);
		if (m_inputSystem->isKeyDown(core::KeyCode::W)) {
			dir.z += 1;
		}
		if (m_inputSystem->isKeyDown(core::KeyCode::S)) {
			dir.z -= 1;
		}
		if (m_inputSystem->isKeyDown(core::KeyCode::A)) {
			dir.x -= 1;
		}
		if (m_inputSystem->isKeyDown(core::KeyCode::D)) {
			dir.x += 1;
		}
		if (m_inputSystem->isKeyDown(core::KeyCode::Space)) {
			dir.y += 1;
		}
		if (m_inputSystem->isKeyDown(core::KeyCode::LeftShift)) {
			dir.y -= 1;
		}

		if (m_inputSystem->isMouseButtonDown(core::MouseButton::Button5)) {
			cameraSettings.cameraSpeed += cameraSettings.cameraSpeedChange * dt;
		} else if (m_inputSystem->isMouseButtonDown(core::MouseButton::Button4)) {
			cameraSettings.cameraSpeed -= cameraSettings.cameraSpeedChange * dt;
		}

		cameraSettings.cameraSpeed =
		    math::clamp(cameraSettings.cameraSpeed, cameraSettings.cameraSpeedMin,
		                cameraSettings.cameraSpeedMax);

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
		entt::registry &registry = getActiveScene()->getRegistry();

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
		m_assetManager = std::make_unique<AssetManager>(m_bus);
		m_scene = std::make_unique<Scene>(m_assetManager.get());
		m_gpuRegistry =
		    std::make_unique<GpuResourceRegistry>(m_renderer, m_bus, m_assetManager.get());

		m_assetManager->registerLoader<Model>(std::make_unique<ModelLoader>(m_assetManager.get()));
		m_assetManager->registerLoader<Texture>(std::make_unique<TextureLoader>());
		m_assetManager->registerLoader<Shader>(std::make_unique<ShaderLoader>());

		registerDefaultAssets();

		RenderSystem::setRegistry(m_gpuRegistry.get());
		RenderSystem::setRenderer(m_renderer);
		RenderSystem::setAssetManager(m_assetManager.get());

		cameraInit();
	}

	TypedAssetID<Texture> genSolidTexture(AssetManager &assetManager, const std::string &name,
	                                      uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		Texture tex;
		tex.width = 1;
		tex.height = 1;
		tex.channels = 4;
		tex.pixels = {r, g, b, a};

		return assetManager.create<Texture>(name, std::move(tex));
	}

	void GameLayer::registerDefaultAssets() {
		TypedAssetID<Texture> ad = genSolidTexture(
		    *m_assetManager, std::string(kDefaultAlbedoTextureName), 255, 255, 255, 255);
		TypedAssetID<Texture> mtd = genSolidTexture(
		    *m_assetManager, std::string(kDefaultMetallicTextureName), 0, 0, 0, 255);
		TypedAssetID<Texture> nd = genSolidTexture(
		    *m_assetManager, std::string(kDefaultNormalTextureName), 128, 128, 255, 255);
		TypedAssetID<Texture> rd = genSolidTexture(
		    *m_assetManager, std::string(kDefaultRoughnessTextureName), 255, 255, 255, 255);

		std::optional<TypedAssetID<Shader>> sd = m_assetManager->load<Shader>(
		    std::string(kDefaultShaderName),
		    ShaderLoadParams{"assets\\shaders\\__default_shader.vert",
		                     "assets\\shaders\\__default_shader.frag"});

		if (!ad.isValid() || !mtd.isValid() || !nd.isValid() || !rd.isValid() || !sd) {
			triple::log::Logger::ModuleCritical(
			    "GameLayer", "The default resources were not loaded properly, and the program "
			                 "cannot continue working normally.");
			return; // TODO: currently just logs, should actually stop startup later
		}

		Material material;
		material.albedoColor = triple::math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		material.albedoTexture = ad;
		material.metallicTexture = mtd;
		material.normalTexture = nd;
		material.roughnessTexture = rd;
		material.shader = *sd;
		material.metallic = 0.1f;
		material.roughness = 1.0f;

		m_assetManager->create<Material>(std::string(kDefaultMaterialName), material);
	}
} // namespace triple::game