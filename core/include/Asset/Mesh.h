#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Asset/Primitive.h"
#include "Asset/Asset.h"

namespace TripleEngineCore::Asset
{
    struct Mesh {
    public:
        std::string name;
        std::vector<Primitive> primitives;
    };
}

#endif // MESH_H
