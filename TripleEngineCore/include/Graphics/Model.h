#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "Mesh.h"
#include "Material.h"

namespace TripleEngineCore::Graphics
{
    class Model {
    public:
        std::vector<Mesh> meshes;
        std::vector<Material> materials;

        static Model CreateCube();
    };
}

#endif // MODEL_H
