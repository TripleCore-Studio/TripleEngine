#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

#include <triple/math/Mat4.h>

#include "triple/game/ecs/CameraComponent.h"
#include "triple/game/ecs/TransformComponent.h"

namespace triple::game {
	triple::math::Mat4 getProjectionMatrix(const CameraComponent &camera);
	triple::math::Mat4 getViewMatrix(const TransformComponent &transform);
} // namespace triple::game

#endif // CAMERA_UTILS_H