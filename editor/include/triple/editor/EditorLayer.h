#ifndef EDITOR_APP_H
#define EDITOR_APP_H

#include <vector>
#include <string>

#include <triple/gfx/LightSources.h>

#include <triple/core/base/Layer.h>
#include <triple/core/input/InputActionSystem.h>

#include <triple/game/GameLayer.h>

#include "triple/editor/UI/ImGuiLayer.h"
#include "triple/editor/UI/UIManager.h"
#include "triple/editor/UI/InspectorPanel.h"
#include "triple/editor/SunLight.h"

namespace triple::editor {
	class EditorLayer : public core::Layer {
	public:
		EditorLayer(game::GameLayer *game) : m_gameLayer(game) {}
		void onAttach(const core::EngineContext &ctx) override;
		void onDetach() override;
		void onUpdate(float dt) override;
		void onRender(float t) override;
		void onEvent(core::Event &e) override;

		void load();
		void loadCallbacks();
		void fillFrameContext(gfx::FrameContext &ctx);

	private:
		std::string uniqueName(game::Scene *scene, const std::string &baseName);

	private:
		math::Vec3 m_ambientColor;
		editor::SunLight m_sunLight;
		gfx::CameraLight m_cameraLight;
		std::vector<std::string> m_loadedModels;

		game::GameLayer *m_gameLayer;
		core::IWindow *m_window = nullptr;
		core::InputActionSystem *m_inputActionSystem = nullptr;

	private:
		ImGuiLayer m_imguiLayer;
		UIManager m_uiManager;
		InspectorPanel *m_inspector;

		bool m_isDebugVisible = true;
		bool m_lastCursorCaptureState = true;
	};
} // namespace triple::editor

#endif // EDITOR_APP_H
