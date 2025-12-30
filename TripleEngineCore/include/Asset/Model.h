#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "Mesh.h"
#include "Asset/Asset.h"

namespace TripleEngineCore::Asset
{
    struct Model : public Asset {
    public:
        Model() = default;

        std::vector<Mesh> meshes;
        std::vector<Graphics::Vertex> vertices;
        std::vector<uint32_t> indices;

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) = default;
        Model& operator=(Model&&) = default;
    };
}

#endif // MODEL_H
