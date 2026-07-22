#pragma once

#include <vector>
#include <string>

#include <entt/entt.hpp>

#include <triple/math/MathCommon.h>

#include <triple/core/event/Event.h>
#include <triple/core/base/Layer.h>
#include <triple/game/GameLayer.h>

#include "triple/editor/UI/UIManager.h"
#include "triple/editor/UI/InspectorPanel.h"
#include "triple/editor/SunLight.h"

namespace triple::editor {
	class ImGuiLayer : public core::Layer {
	public:
		ImGuiLayer(game::GameLayer *gameLayer, SunLight *sunLight, math::Vec3 *ambientColor,
		           std::vector<std::string> *loadedModels)
		    : m_gameLayer(gameLayer), m_sunLight(sunLight), m_ambientColor(ambientColor),
		      m_loadedModels(loadedModels) {}

		~ImGuiLayer();
		void onAttach(const core::EngineContext &ctx) override;
		void onDetach() override;
		void onUpdate(float dt) override;
		void onRender(float t) override;
		void onEvent(core::Event &e) override;

		void resize(int width, int height);
		void show();
		void hide();

		bool opened = true;

	private:
		void beginFrame();
		void endFrame();
		void stylesApply();
		void configureApply();
		void load();

	private:
		UIManager m_uiManager;
		InspectorPanel *m_inspector = nullptr;
		bool m_initialized = false;

		game::GameLayer *m_gameLayer = nullptr;
		SunLight *m_sunLight = nullptr;
		math::Vec3 *m_ambientColor = nullptr;
		std::vector<std::string> *m_loadedModels = nullptr;
	};
} // namespace triple::editor
