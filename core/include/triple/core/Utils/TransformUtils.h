#ifndef TRANSFORM_UTILS_H
#define TRANSFORM_UTILS_H

#include <triple/math/Mat4.h>
#include "triple/core/ExportMacros.h"
#include "triple/core/Scene/TransformComponent.h"

namespace triple::core {
	CORE_API triple::math::Mat4 getModelMatrix(const TransformComponent& t);
	CORE_API triple::math::Mat4 getRotationMatrix(const TransformComponent& t);
	CORE_API triple::math::Vec3 forward(const TransformComponent& t);
	CORE_API triple::math::Vec3 right(const TransformComponent& t);
	CORE_API triple::math::Vec3 up(const TransformComponent& t);
}

#endif // TRANSFORM_UTILS_H