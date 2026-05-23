#ifndef EDITOR_APP_H
#define EDITOR_APP_H

#include <triple/core/base/Engine.h>

namespace triple::editor {
	class EditorApp : public core::Engine {
	  public:
		void onUpdate(float dt) override;
		void cameraUpdate(float dt);
		void demoScene();
		void loadCallbacks();

	  private:
		struct CameraSettings {
			float cameraSpeed = 8.0f;
			float cameraSpeedChange = 20.0f;
			float cameraSpeedMin = 1.0f;
			float cameraSpeedMax = 1000.0f;

			float sensitivity = 0.15f;
		} _cameraSettings;
	};
} // namespace triple::editor

#endif // EDITOR_APP_H
