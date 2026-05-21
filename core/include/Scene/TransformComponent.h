#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "ExportMacros.h"
#include "Vec3.h"

namespace TripleEngineCore::Scene {
    struct TransformComponent {
        TripleMath::Vec3 position{ 0,0,0 };
        TripleMath::Vec3 rotationEuler{ 0,0,0 };
        TripleMath::Vec3 scale{ 1,1,1 };
    };
}

#endif // TRANSFORM_COMPONENT_H
