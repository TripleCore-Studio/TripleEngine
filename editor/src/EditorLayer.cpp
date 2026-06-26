#include "triple/editor/EditorLayer.h"

#include <triple/math/MathCommon.h>

using namespace triple::core;
using namespace triple::math;

namespace triple::editor {
	void EditorLayer::onAttach(const core::EngineContext &ctx) {
		m_window = ctx.window;
		m_actionMap = std::make_unique<core::ActionMap>(ctx.inputSystem);

		ambientColor = Vec3(0.53f, 0.65f, 0.85f);

		sunLight.color = Vec3(1.0f, 0.95f, 0.8f);
		sunLight.azimuth = 40.0f;
		sunLight.elevation = 45.0f;
		sunLight.intensity = 1.8f;

		cameraLight.color = Vec3(0.9f, 0.95f, 1.0f);
		cameraLight.intensity = 7.0f;
		cameraLight.radius = 1.0f;

		loadCallbacks();
	}

	void EditorLayer::onDetach() {}

	void EditorLayer::onUpdate(float dt) { m_actionMap->update(); }

	void EditorLayer::onRender(float t) {}

	void EditorLayer::onEvent(core::Event &e) {}

	void EditorLayer::fillFrameContext(gfx::FrameContext &ctx) const {
		ctx.ambientColor = ambientColor;
		ctx.cameraLight = cameraLight;
		ctx.sunLight.color = sunLight.color;
		ctx.sunLight.intensity = sunLight.intensity;

		float elRad = math::radians(sunLight.elevation);
		float azRad = math::radians(sunLight.azimuth);
		ctx.sunLight.direction = math::normalize(Vec3(
		    std::cos(elRad) * std::sin(azRad), std::sin(elRad), std::cos(elRad) * std::cos(azRad)));
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
				m_imguiLayer->hide();
				m_isDebugVisible = false;
			} else {
				m_lastCursorCaptureState = m_window->isCursorCaptured();
				m_window->setCursorCapture(false);
				m_imguiLayer->show();
				m_isDebugVisible = true;
			}
		});
	}
} // namespace triple::editor
