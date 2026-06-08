#pragma once

namespace triple::game {
	struct CameraSettings {
		float cameraSpeed = 8.0f;
		float cameraSpeedChange = 20.0f;
		float cameraSpeedMin = 1.0f;
		float cameraSpeedMax = 1000.0f;
		float sensitivity = 0.15f;
		bool lockY = true;
	};
} // namespace triple::game