#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include <triple/math/Mat4.h>

namespace triple::game {
	struct CameraComponent {
		float fov;
		float nearPlane;
		float farPlane;
		float aspectRatio;

		triple::math::Mat4 projectionMatrix{1.0f};
		triple::math::Mat4 viewMatrix{1.0f};
	};
} // namespace triple::game

#endif // CAMERA_COMPONENT_H
