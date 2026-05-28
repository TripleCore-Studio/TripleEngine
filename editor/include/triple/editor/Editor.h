#ifndef EDITOR_APP_H
#define EDITOR_APP_H

#include <vector>
#include <string>

#include <triple/core/base/Engine.h>

#include "triple/editor/UI/ImGuiLayer.h"
#include "triple/editor/UI/UIManager.h"
#include "triple/editor/UI/InspectorPanel.h"

namespace triple::editor {
	class Editor : public core::Engine {
	public:
		void onUpdate(float dt) override;
		void onRenderUI(float dt) override;
		void cameraUpdate(float dt);
		void onLoaded();
		void loadCallbacks();

	private:
		void cameraInit();
		std::string uniqueName(core::Scene *scene, const std::string &baseName);

		struct CameraSettings {
			float cameraSpeed = 8.0f;
			float cameraSpeedChange = 20.0f;
			float cameraSpeedMin = 1.0f;
			float cameraSpeedMax = 1000.0f;
			float sensitivity = 0.15f;
			bool lockY = true;
		} m_cameraSettings;

		core::Entity m_camera;
		std::vector<std::string> m_loadedModels;

	private:
		ImGuiLayer m_imguiLayer;
		UIManager m_uiManager;
		InspectorPanel *m_inspector;
		bool m_isDebugVisible = true;
		bool m_lastCursorCaptureState = true;
	};
} // namespace triple::editor

#endif // EDITOR_APP_H
