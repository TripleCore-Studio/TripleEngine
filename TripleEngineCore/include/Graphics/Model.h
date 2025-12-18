#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "Mesh.h"
#include "Material.h"
#include <memory>

namespace TripleEngineCore::Graphics
{
    class Model {
    public:
        Model() = default;

        std::vector<std::unique_ptr<Mesh>> meshes;

        static Model CreateCube();
        static Model LoadOBJ(const std::string& path);

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) = default;
        Model& operator=(Model&&) = default;
    };
}

#endif // MODEL_H
