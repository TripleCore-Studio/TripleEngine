#ifndef CHILDREN_COMPONENT_H
#define CHILDREN_COMPONENT_H

#include <vector>
#include "Scene/Scene.h"

namespace TripleEngineCore::Scene {
    struct ChildrenComponent {
        std::vector<Entity> children;
    };
}

#endif // CHILDREN_COMPONENT_H
