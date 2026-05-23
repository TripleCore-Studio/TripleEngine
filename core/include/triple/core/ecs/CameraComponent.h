#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

namespace triple::core {
	struct CameraComponent {
		float fov;
		float nearPlane;
		float farPlane;
		float aspectRatio;
	};
} // namespace triple::core

#endif // CAMERA_COMPONENT_H
