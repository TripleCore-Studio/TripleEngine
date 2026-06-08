#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

namespace triple::game {
	struct CameraComponent {
		float fov;
		float nearPlane;
		float farPlane;
		float aspectRatio;
	};
} // namespace triple::game

#endif // CAMERA_COMPONENT_H
