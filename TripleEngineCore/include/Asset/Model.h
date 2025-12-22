#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "Mesh.h"

namespace TripleEngineCore::Asset
{
    class Model {
    public:
        Model() = default;

        std::vector<Mesh> meshes;

        static Model CreateCube();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) = default;
        Model& operator=(Model&&) = default;
    };
}

#endif // MODEL_H
