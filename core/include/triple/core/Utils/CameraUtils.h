#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

#include "triple/core/Scene/CameraComponent.h"
#include "triple/core/Scene/TransformComponent.h"
#include <triple/math/Mat4.h>

namespace triple::core {
	triple::math::Mat4 getProjectionMatrix(const CameraComponent& camera);
	triple::math::Mat4 getViewMatrix(const TransformComponent& transform);
}

#endif // CAMERA_UTILS_H