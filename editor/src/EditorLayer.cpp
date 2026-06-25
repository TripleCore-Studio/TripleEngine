#include "triple/editor/EditorLayer.h"

#include <filesystem>

#include <triple/math/MathCommon.h>

#include <triple/game/utils/HierarchyUtils.h>

#include <triple/game/ecs/NameComponent.h>
#include <triple/game/ecs/MeshComponent.h>

#include "triple/editor/UI/HierarchyPanel.h"
#include "triple/editor/UI/DescPanel.h"

using namespace triple::core;
using namespace triple::math;

namespace triple::editor {
	void EditorLayer::onAttach(const core::EngineContext &ctx) {
		m_window = ctx.window;
		m_actionMap = std::make_unique<core::ActionMap>(ctx.inputSystem);
		load();
		loadCallbacks();
	}

	void EditorLayer::onDetach() {}

	void EditorLayer::onUpdate(float dt) {
		m_actionMap->update();
		m_dt = dt;
	}

	void EditorLayer::onRender(float t) {
		m_imguiLayer.beginFrame(m_dt);
		m_uiManager.render();
		m_imguiLayer.endFrame();
	}

	void EditorLayer::onEvent(core::Event &e) { m_imguiLayer.onEvent(e); }

	void EditorLayer::load() {
		m_ambientColor = Vec3(0.53f, 0.65f, 0.85f);

		m_sunLight.color = Vec3(1.0f, 0.95f, 0.8f);
		m_sunLight.azimuth = 40.0f;
		m_sunLight.elevation = 45.0f;
		m_sunLight.intensity = 1.8f;

		m_cameraLight.color = Vec3(0.9f, 0.95f, 1.0f);
		m_cameraLight.intensity = 7.0f;
		m_cameraLight.radius = 1.0f;

		m_imguiLayer.init();
		m_imguiLayer.resize(m_window->getWidth(), m_window->getHeight());

		m_uiManager.addPanel<HierarchyPanel>(
		    m_gameLayer->getActiveScene(),
		    [this](entt::entity e) { m_inspector->setTarget(e, m_gameLayer->getActiveScene()); },
		    [this](std::string path) {
			    entt::registry &registry = m_gameLayer->getActiveScene()->getRegistry();

			    entt::entity entity = registry.create();
			    std::string baseName = std::filesystem::path(path).stem().string();
			    std::string name = uniqueName(registry, baseName);

			    registry.emplace<game::NameComponent>(entity, name);
			    registry.emplace<game::TransformComponent>(entity);

			    if (!m_loadedModels.empty()) {
				    for (auto &modelStr : m_loadedModels) {
					    game::ModelID modelId =
					        m_gameLayer->getAssetService()->getModelId(modelStr);
					    if (modelId != game::INVALID_ASSET_ID) {
						    registry.emplace<game::MeshComponent>(entity, modelId);
					    }
				    }
			    } else {
				    registry.emplace<game::MeshComponent>(
				        entity, m_gameLayer->getAssetService()->loadModelFromFile(baseName, path));
			    }
		    },
		    [this](entt::entity e) {
			    if (e != m_gameLayer->getActiveCamera()) {
				    game::HierarchyUtils::destroyEntityRecursive(
				        m_gameLayer->getActiveScene()->getRegistry(), e);
			    }
		    },
		    [this](entt::entity e) {
			    if (e == m_gameLayer->getActiveCamera())
				    return;

			    entt::registry &registry = m_gameLayer->getActiveScene()->getRegistry();

			    game::TransformComponent currentTransform =
			        registry.get<game::TransformComponent>(e);
			    game::MeshComponent currentMesh = registry.get<game::MeshComponent>(e);
			    game::NameComponent currentName = registry.get<game::NameComponent>(e);

			    std::string newName = uniqueName(registry, currentName.name);

			    entt::entity newEntity = registry.create();
			    game::TransformComponent &newTransform =
			        registry.emplace<game::TransformComponent>(newEntity);

			    newTransform.position = currentTransform.position + Vec3(1, 1, 1);
			    newTransform.rotationEuler = currentTransform.rotationEuler;
			    newTransform.scale = currentTransform.scale;

			    registry.emplace<game::MeshComponent>(newEntity, currentMesh.modelIndex);
			    registry.emplace<game::NameComponent>(newEntity, newName);
		    });

		m_inspector = m_uiManager.addPanel<InspectorPanel>();
		m_inspector->setCamera(
		    &m_gameLayer->cameraSettings.cameraSpeed, m_gameLayer->cameraSettings.cameraSpeedMin,
		    m_gameLayer->cameraSettings.cameraSpeedMax, &m_gameLayer->cameraSettings.lockY);
		m_inspector->setLight(&m_sunLight, &m_cameraLight, &m_ambientColor);

		m_uiManager.addPanel<DescPanel>();

		if (!m_isDebugVisible) {
			m_uiManager.hideAll();
		}
	}

	void EditorLayer::fillFrameContext(gfx::FrameContext &ctx) {
		ctx.ambientColor = m_ambientColor;
		ctx.cameraLight = m_cameraLight;
		ctx.sunLight.color = m_sunLight.color;
		ctx.sunLight.intensity = m_sunLight.intensity;

		float elRad = math::radians(m_sunLight.elevation);
		float azRad = math::radians(m_sunLight.azimuth);
		ctx.sunLight.direction = math::normalize(Vec3(
		    std::cos(elRad) * std::sin(azRad), std::sin(elRad), std::cos(elRad) * std::cos(azRad)));
	}

	std::string EditorLayer::uniqueName(entt::registry &registry, const std::string &baseName) {
		std::string name = baseName;
		int counter = 1;

		while (true) {
			bool found = false;

			auto view = registry.view<game::NameComponent>();
			for (auto [entity, n] : view.each()) {
				if (n.name == name) {
					found = true;
					break;
				}
			}

			if (!found)
				return name;

			name = baseName + "(" + std::to_string(counter++) + ")";
		}
	}

	void EditorLayer::loadCallbacks() {

		InputTrigger fullscreenTrigger;
		fullscreenTrigger.type = InputTriggerType::Key;
		fullscreenTrigger.state = TriggerState::Pressed;
		fullscreenTrigger.key = KeyCode::F11;

		m_actionMap->bind("ToggleFullscreen", {fullscreenTrigger},
		                  [this]() { m_window->setFullscreen(!m_window->isFullscreen()); });

		InputTrigger captureMouseTrigger;
		captureMouseTrigger.type = InputTriggerType::Key;
		captureMouseTrigger.state = TriggerState::Pressed;
		captureMouseTrigger.key = KeyCode::F10;

		m_actionMap->bind("ToggleCaptureMouse", {captureMouseTrigger},
		                  [this]() { m_window->setCursorCapture(!m_window->isCursorCaptured()); });

		InputTrigger debugMenuTrigger;
		debugMenuTrigger.type = InputTriggerType::Key;
		debugMenuTrigger.state = TriggerState::Pressed;
		debugMenuTrigger.key = KeyCode::F9;
		m_actionMap->bind("ToggleDebugMenu", {debugMenuTrigger}, [this]() {
			if (m_isDebugVisible) {
				m_window->setCursorCapture(m_lastCursorCaptureState);
				m_uiManager.hideAll();
				m_imguiLayer.opened = false;
				m_isDebugVisible = false;
			} else {
				m_lastCursorCaptureState = m_window->isCursorCaptured();
				m_window->setCursorCapture(false);
				m_uiManager.showAll();
				m_imguiLayer.opened = true;
				m_isDebugVisible = true;
			}
		});
	}
} // namespace triple::editor