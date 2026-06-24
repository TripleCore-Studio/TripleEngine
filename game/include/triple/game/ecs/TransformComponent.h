#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <triple/math/Vec3.h>
#include <triple/math/Mat4.h>

namespace triple::game {
	struct TransformComponent {
		triple::math::Vec3 position{0, 0, 0};
		triple::math::Vec3 rotationEuler{0, 0, 0};
		triple::math::Vec3 scale{1, 1, 1};

		triple::math::Mat4 worldMatrix;
	};
} // namespace triple::game

#endif // TRANSFORM_COMPONENT_H
