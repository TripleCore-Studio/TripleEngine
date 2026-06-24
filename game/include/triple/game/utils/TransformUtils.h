#ifndef TRANSFORM_UTILS_H
#define TRANSFORM_UTILS_H

#include <triple/math/Mat4.h>

#include "triple/game/ecs/TransformComponent.h"

namespace triple::game {
	class TransformUtils {
	public:
		static triple::math::Mat4 getModelMatrix(const TransformComponent &t);
		static triple::math::Mat4 getRotationMatrix(const TransformComponent &t);
		static triple::math::Vec3 forward(const TransformComponent &t);
		static triple::math::Vec3 right(const TransformComponent &t);
		static triple::math::Vec3 up(const TransformComponent &t);
	};
} // namespace triple::game

#endif // TRANSFORM_UTILS_H