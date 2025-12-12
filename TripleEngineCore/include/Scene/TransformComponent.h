#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "Component.h"
#include "Vec3.h"
#include "Mat4.h"

namespace TripleEngineCore::Scene {

    class TransformComponent : public Component {
    public:
        TransformComponent(TripleMath::Vec3 p, TripleMath::Vec3 r, TripleMath::Vec3 s) : position(p), rotationEuler(r), scale(s) {}
        TransformComponent() {}
        TripleMath::Vec3 position{ 0, 0, 0 };
        TripleMath::Vec3 rotationEuler{ 0, 0, 0 };
        TripleMath::Vec3 scale{ 1, 1, 1 };

        TripleMath::Mat4 getModelMatrix() const;
        TripleMath::Vec3 forward() const;
        TripleMath::Vec3 right() const;
        TripleMath::Vec3 up() const;
    };
}

#endif // TRANSFORM_COMPONENT_H
