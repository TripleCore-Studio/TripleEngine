#include "triple/editor/Editor.h"

#include <filesystem>

#include <triple/core/ecs/TransformComponent.h>
#include <triple/core/ecs/CameraComponent.h>
#include <triple/core/ecs/MeshComponent.h>
#include <triple/core/ecs/Scene.h>

#include <triple/math/MathCommon.h>
#include <triple/core/utils/TransformUtils.h>
#include <triple/core/event/EngineLoadedEvent.h>

#include <triple/core/input/InputSystem.h>
#include <triple/core/input/InputActionSystem.h>

#include <triple/core/asset/AssetService.h>
#include <triple/core/event/EventService.h>

#include "triple/editor/UI/HierarchyPanel.h"
#include "triple/editor/UI/DescPanel.h"

using namespace triple::core;
using namespace triple::math;

namespace triple::editor {
	void Editor::onUpdate(float dt) { cameraUpdate(dt); }

	void Editor::onRenderUI(float dt) {
		m_imguiLayer.beginFrame();
		m_uiManager.render();
		m_imguiLayer.endFrame();
	}

	void Editor::cameraInit() {
		Scene *scene = getActiveScene();

		m_camera = scene->createEntity("camera");

		TransformComponent *transform = scene->addComponent<TransformComponent>(m_camera);
		transform->position = Vec3(0, 0, 0);
		transform->rotationEuler = Vec3(0, 0, 0);
		transform->scale = Vec3(1, 1, 1);

		CameraComponent *camera = scene->addComponent<CameraComponent>(m_camera);
		camera->aspectRatio = 16.0f / 9.0f;
		camera->fov = 70.0f;
		camera->farPlane = 1000.0f;
		camera->nearPlane = 0.01f;

		setActiveCamera(m_camera);
	}

	void Editor::cameraUpdate(float dt) {
		if (m_isDebugVisible)
			return;

		TransformComponent *cameraTransform =
		    getActiveScene()->getComponent<TransformComponent>(m_camera);
		if (!cameraTransform) {
			return;
		}

		InputSystem *input = getSystem<InputSystem>();

		Vec2 delta = input->getMouseDelta();
		cameraTransform->rotationEuler.y += -delta.x * m_cameraSettings.sensitivity; // yaw
		cameraTransform->rotationEuler.x += delta.y * m_cameraSettings.sensitivity;  // pitch
		cameraTransform->rotationEuler.x = clamp(cameraTransform->rotationEuler.x, -89.0f, 89.0f);

		Vec3 dir(0, 0, 0);
		if (input->isKeyDown(KeyCode::W)) {
			dir.z += 1;
		}
		if (input->isKeyDown(KeyCode::S)) {
			dir.z -= 1;
		}
		if (input->isKeyDown(KeyCode::A)) {
			dir.x -= 1;
		}
		if (input->isKeyDown(KeyCode::D)) {
			dir.x += 1;
		}
		if (input->isKeyDown(KeyCode::Space)) {
			dir.y += 1;
		}
		if (input->isKeyDown(KeyCode::LeftShift)) {
			dir.y -= 1;
		}

		if (input->isMouseButtonDown(MouseButton::Button5)) {
			m_cameraSettings.cameraSpeed += m_cameraSettings.cameraSpeedChange * dt;
		} else if (input->isMouseButtonDown(MouseButton::Button4)) {
			m_cameraSettings.cameraSpeed -= m_cameraSettings.cameraSpeedChange * dt;
		}

		m_cameraSettings.cameraSpeed =
		    clamp(m_cameraSettings.cameraSpeed, m_cameraSettings.cameraSpeedMin,
		          m_cameraSettings.cameraSpeedMax);

		if (dir.length() > 0) {
			dir = dir.normalized() * m_cameraSettings.cameraSpeed * dt;

			Vec3 fwd = forward(*cameraTransform);
			if (m_cameraSettings.lockY) {
				fwd.y = 0;
				fwd = fwd.normalized();
			}
			Vec3 rightVec = right(*cameraTransform);
			Vec3 up(0, 1, 0);

			cameraTransform->position += fwd * dir.z;
			cameraTransform->position += rightVec * dir.x;
			cameraTransform->position += up * dir.y;
		}
	}

	std::string Editor::uniqueName(Scene *scene, const std::string &baseName) {
		auto entities = scene->getEntities();
		std::string name = baseName;
		int counter = 1;

		while (true) {
			bool found = false;
			for (auto &e : entities) {
				auto *n = scene->getComponent<NameComponent>(e);
				if (n && n->name == name) {
					found = true;
					break;
				}
			}
			if (!found)
				return name;
			name = baseName + "(" + std::to_string(counter++) + ")";
		}
	}

	void Editor::onLoaded() {
		m_ambientColor = Vec3(0.53f, 0.65f, 0.85f);

		m_sunLight.color = Vec3(1.0f, 0.95f, 0.8f);
		m_sunLight.azimuth = 40.0f;
		m_sunLight.elevation = 45.0f;
		m_sunLight.intensity = 1.8f;

		m_cameraLight.color = Vec3(0.9f, 0.95f, 1.0f);
		m_cameraLight.intensity = 7.0f;
		m_cameraLight.radius = 1.0f;

		cameraInit();
		m_imguiLayer.init(getWindow());

		m_uiManager.addPanel<HierarchyPanel>(
		    getActiveScene(),
		    [this](core::Entity e) { m_inspector->setTarget(e, getActiveScene()); },
		    [this](std::string path) {
			    Scene *scene = getActiveScene();
			    Entity entity = scene->createEntity();
			    std::string baseName = std::filesystem::path(path).stem().string();
			    std::string name = uniqueName(scene, baseName);

			    scene->addComponent<NameComponent>(entity)->name = name;
			    scene->addComponent<TransformComponent>(entity)->scale = Vec3(1, 1, 1);

			    if (!m_loadedModels.empty()) {
				    for (auto &modelStr : m_loadedModels) {
					    ModelID modelId = getService<AssetService>()->getModelId(modelStr);
					    if (modelId != INVALID_ASSET_ID) {
						    scene->addComponent<MeshComponent>(entity)->modelIndex = modelId;
					    }
				    }
			    } else {
				    scene->addComponent<MeshComponent>(entity)->modelIndex =
				        getService<AssetService>()->loadModelFromFile(baseName, path);
			    }
		    },
		    [this](core::Entity e) {
			    if (e != m_camera) {
				    getActiveScene()->destroyEntity(e);
			    }
		    },
		    [this](core::Entity e) {
			    if (e == m_camera)
				    return;

			    Scene *scene = getActiveScene();
			    TransformComponent currentTransform = *scene->getComponent<TransformComponent>(e);
			    MeshComponent currentMesh = *scene->getComponent<MeshComponent>(e);
			    NameComponent currentName = *scene->getComponent<NameComponent>(e);

			    std::string newName = uniqueName(scene, currentName.name);

			    Entity newEntity = scene->createEntity();
			    TransformComponent *newTransform =
			        scene->addComponent<TransformComponent>(newEntity);

			    newTransform->position = currentTransform.position + Vec3(1, 1, 1);
			    newTransform->rotationEuler = currentTransform.rotationEuler;
			    newTransform->scale = currentTransform.scale;

			    scene->addComponent<MeshComponent>(newEntity)->modelIndex = currentMesh.modelIndex;
			    scene->addComponent<NameComponent>(newEntity)->name = newName;
		    });

		m_inspector = m_uiManager.addPanel<InspectorPanel>();
		m_inspector->setCamera(&m_cameraSettings.cameraSpeed, m_cameraSettings.cameraSpeedMin,
		                       m_cameraSettings.cameraSpeedMax, &m_cameraSettings.lockY);
		m_inspector->setLight(&m_sunLight, &m_cameraLight, &m_ambientColor);

		m_uiManager.addPanel<DescPanel>();

		onFrame = [this](gfx::FrameContext &ctx) {
			ctx.ambientColor = m_ambientColor;
			ctx.cameraLight = m_cameraLight;

			ctx.sunLight.color = m_sunLight.color;
			ctx.sunLight.intensity = m_sunLight.intensity;
			float elRad = math::radians(m_sunLight.elevation);
			float azRad = math::radians(m_sunLight.azimuth);
			ctx.sunLight.direction =
			    math::normalize(Vec3(std::cos(elRad) * std::sin(azRad), std::sin(elRad),
			                         std::cos(elRad) * std::cos(azRad)));
		};
	}

	void Editor::loadCallbacks() {
		getService<EventService>()->addListener<EngineLoadedEvent>(
		    [this](EngineLoadedEvent &e) { onLoaded(); });

		InputTrigger fullscreenTrigger;
		fullscreenTrigger.type = InputTriggerType::Key;
		fullscreenTrigger.state = TriggerState::Pressed;
		fullscreenTrigger.key = KeyCode::F11;

		getSystem<InputActionSystem>()->bind("ToggleFullscreen", {fullscreenTrigger}, [this]() {
			getWindow()->setFullscreen(!getWindow()->isFullscreen());
		});

		InputTrigger captureMouseTrigger;
		captureMouseTrigger.type = InputTriggerType::Key;
		captureMouseTrigger.state = TriggerState::Pressed;
		captureMouseTrigger.key = KeyCode::F10;

		getSystem<InputActionSystem>()->bind("ToggleCaptureMouse", {captureMouseTrigger}, [this]() {
			getWindow()->setCursorCapture(!getWindow()->isCursorCaptured());
		});

		InputTrigger debugMenuTrigger;
		debugMenuTrigger.type = InputTriggerType::Key;
		debugMenuTrigger.state = TriggerState::Pressed;
		debugMenuTrigger.key = KeyCode::F9;
		getSystem<InputActionSystem>()->bind("ToggleDebugMenu", {debugMenuTrigger}, [this]() {
			if (m_isDebugVisible) {
				getWindow()->setCursorCapture(m_lastCursorCaptureState);
				m_uiManager.hideAll();
				m_isDebugVisible = false;
			} else {
				m_lastCursorCaptureState = getWindow()->isCursorCaptured();
				getWindow()->setCursorCapture(false);
				m_uiManager.showAll();
				m_isDebugVisible = true;
			}
		});
	}
} // namespace triple::editor