#include <triple/game/GameLayer.h>

#include <triple/math/MathCommon.h>

#include <triple/log/Logger.h>

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/CameraComponent.h"
#include "triple/game/ecs/MeshComponent.h"
#include "triple/game/ecs/ChildrenComponent.h"
#include "triple/game/ecs/ParentComponent.h"
#include "triple/game/ecs/NameComponent.h"

#include "triple/game/utils/CameraUtils.h"
#include "triple/game/utils/TransformUtils.h"

namespace triple::game {
	void GameLayer::onAttach(const core::EngineContext &ctx) {
		m_renderer = ctx.renderer;
		m_inputSystem = ctx.inputSystem;

		m_assetService = std::make_unique<AssetService>();

		m_renderSystem = std::make_unique<RenderSystem>(m_assetService.get());
		m_renderSystem->setRenderer(m_renderer);

		m_componentService = std::make_unique<ComponentService>();
		m_scene = std::make_unique<Scene>(m_componentService.get());

		if (!bootstrapComponents()) {
			triple::log::Logger::ModuleCritical("GameLayer", "Error: bootstrapComponents");
		}

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
	void GameLayer::onUpdate(float dt) { cameraUpdate(dt); }
	void GameLayer::onRender(float t) {
		auto *transform = m_scene->getComponent<TransformComponent>(m_cameraEntity);
		auto *cameraComp = m_scene->getComponent<CameraComponent>(m_cameraEntity);

		if (!transform || !cameraComp) {
			triple::log::Logger::ModuleWarn("GameLayer", "Active camera missing on render");
			return;
		}

		gfx::FrameContext ctx;

		ctx.camera = gfx::CameraData{getViewMatrix(*transform), getProjectionMatrix(*cameraComp),
		                             transform->position};

		m_renderSystem->buildRenderCommands(m_scene.get(), ctx.commands);
		ctx.time = t;

		if (m_onFrameContext)
			m_onFrameContext(ctx);

		m_renderer->RenderFrame(ctx);
	}
	void GameLayer::onEvent(core::Event &e) {}
	bool GameLayer::bootstrapComponents() const {
		std::vector<ComponentTypeID> ids;
		ids.push_back(m_componentService->registerComponent<TransformComponent>());
		ids.push_back(m_componentService->registerComponent<MeshComponent>());
		ids.push_back(m_componentService->registerComponent<CameraComponent>());
		ids.push_back(m_componentService->registerComponent<ParentComponent>());
		ids.push_back(m_componentService->registerComponent<ChildrenComponent>());
		ids.push_back(m_componentService->registerComponent<NameComponent>());

		for (auto &id : ids) {
			if (id == INVALID_COMPONENT_TYPE_ID) {
				return false;
			}
		}

		return true;
	}
	void GameLayer::cameraUpdate(float dt) {
		TransformComponent *cameraTransform =
		    m_scene->getComponent<TransformComponent>(m_cameraEntity);

		if (!cameraTransform) {
			return;
		}

		math::Vec2 delta = m_inputSystem->getMouseDelta();
		cameraTransform->rotationEuler.y += -delta.x * cameraSettings.sensitivity; // yaw
		cameraTransform->rotationEuler.x += delta.y * cameraSettings.sensitivity;  // pitch
		cameraTransform->rotationEuler.x =
		    math::clamp(cameraTransform->rotationEuler.x, -89.0f, 89.0f);

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

			math::Vec3 fwd = forward(*cameraTransform);
			if (cameraSettings.lockY) {
				fwd.y = 0;
				fwd = fwd.normalized();
			}
			math::Vec3 rightVec = right(*cameraTransform);
			math::Vec3 up(0, 1, 0);

			cameraTransform->position += fwd * dir.z;
			cameraTransform->position += rightVec * dir.x;
			cameraTransform->position += up * dir.y;
		}
	}
	void GameLayer::cameraInit() {
		Scene *scene = getActiveScene();

		m_cameraEntity = scene->createEntity("camera");

		TransformComponent *transform = scene->addComponent<TransformComponent>(m_cameraEntity);
		transform->position = math::Vec3(0, 0, 0);
		transform->rotationEuler = math::Vec3(0, 0, 0);
		transform->scale = math::Vec3(1, 1, 1);

		CameraComponent *camera = scene->addComponent<CameraComponent>(m_cameraEntity);
		camera->aspectRatio = 16.0f / 9.0f;
		camera->fov = 70.0f;
		camera->farPlane = 1000.0f;
		camera->nearPlane = 0.01f;

		setActiveCamera(m_cameraEntity);
	}
} // namespace triple::game