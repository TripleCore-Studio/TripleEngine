#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Vertex.h"

namespace TripleEngineCore::Graphics
{
    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        uint32_t materialIndex = UINT32_MAX;
    };
}

#endif // MESH_H
