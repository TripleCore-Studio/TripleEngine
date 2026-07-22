#ifndef EDITOR_APP_H
#define EDITOR_APP_H

#include <vector>
#include <string>
#include <memory>

#include <triple/core/base/Layer.h>
#include <triple/core/input/ActionMap.h>

#include <triple/game/GameLayer.h>

#include "triple/editor/UI/ImGuiLayer.h"
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

		void setImGuiLayer(ImGuiLayer *layer) { m_imguiLayer = layer; }
		void loadCallbacks();

		std::vector<std::string> loadedModels;
		math::Vec3 ambientColor;
		editor::SunLight sunLight;

	private:
		game::GameLayer *m_gameLayer = nullptr;
		ImGuiLayer *m_imguiLayer = nullptr;
		core::IWindow *m_window = nullptr;
		std::unique_ptr<core::ActionMap> m_actionMap;

		bool m_isDebugVisible = true;
		bool m_lastCursorCaptureState = true;
	};
} // namespace triple::editor

#endif // EDITOR_APP_H
