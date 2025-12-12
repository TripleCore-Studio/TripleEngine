#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "Component.h"
#include "Graphics/Model.h"

namespace TripleEngineCore::Scene {
    class MeshComponent : public Component {
    public:
        Graphics::Model model;

        MeshComponent() = default;
        MeshComponent(const Graphics::Model& m) : model(m) {}

        void onUpdate(float dt) override {}
    };
}

#endif // MESH_COMPONENT_H