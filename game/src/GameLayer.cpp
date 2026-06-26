#include <triple/game/GameLayer.h>

#include <triple/math/MathCommon.h>

#include <triple/log/Logger.h>

#include "triple/game/utils/TransformUtils.h"

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/CameraComponent.h"

namespace triple::game {
	void GameLayer::onAttach(const core::EngineContext &ctx) {
		m_renderer = ctx.renderer;
		m_inputSystem = ctx.inputSystem;

		m_assetService = std::make_unique<AssetService>();

		m_renderSystem = std::make_unique<RenderSystem>(m_assetService.get());
		m_renderSystem->setRenderer(m_renderer);

		m_scene = std::make_unique<Scene>(m_assetService.get());

		m_assetService->setTextureLoadedCallback(
		    [this](const Texture *tex) { m_renderSystem->uploadTexture(tex); });

		m_assetService->setModelLoadedCallback(
		    [this](const Model *model) { m_renderSystem->uploadGeometry(model); });

		m_assetService->setShaderLoadedCallback(
		    [this](const Shader *shader) { m_renderSystem->uploadShader(shader); });

		m_assetService->loadDefaultAssets();

		cameraInit();
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

		m_renderSystem->buildRenderCommands(registry, ctx.commands);
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
} // namespace triple::game