#include "triple/core/Utils/CameraUtils.h"
#include <triple/math/Mat4Operations.h>
#include "triple/core/Utils/TransformUtils.h"

namespace triple::core {
	triple::math::Mat4 getProjectionMatrix(const CameraComponent &camera) {
		return triple::math::perspective(camera.fov * 0.01745329251f, camera.aspectRatio,
		                                 camera.nearPlane, camera.farPlane);
	}
	triple::math::Mat4 getViewMatrix(const TransformComponent &transform) {
		triple::math::Vec3 eye = transform.position;
		triple::math::Vec3 center = eye + forward(transform);
		triple::math::Vec3 upVec = up(transform);
		return triple::math::lookAt(eye, center, upVec);
	}
} // namespace triple::core