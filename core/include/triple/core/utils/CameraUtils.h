#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

#include "triple/core/ecs/CameraComponent.h"
#include "triple/core/ecs/TransformComponent.h"
#include <triple/math/Mat4.h>

namespace triple::core {
	triple::math::Mat4 getProjectionMatrix(const CameraComponent &camera);
	triple::math::Mat4 getViewMatrix(const TransformComponent &transform);
} // namespace triple::core

#endif // CAMERA_UTILS_H