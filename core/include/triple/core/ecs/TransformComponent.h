#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <triple/math/Vec3.h>

namespace triple::core {
    struct TransformComponent {
        triple::math::Vec3 position{ 0,0,0 };
        triple::math::Vec3 rotationEuler{ 0,0,0 };
        triple::math::Vec3 scale{ 1,1,1 };
    };
}

#endif // TRANSFORM_COMPONENT_H
