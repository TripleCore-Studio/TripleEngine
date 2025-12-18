#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "Component.h"
#include "Graphics/Model.h"
#include <memory>

namespace TripleEngineCore::Scene {
    class MeshComponent : public Component {
    public:
		MeshComponent(uint32_t modelIdx) : modelIndex(modelIdx) {}
        uint32_t modelIndex = UINT32_MAX;

        MeshComponent() = default;
    };
}

#endif // MESH_COMPONENT_H