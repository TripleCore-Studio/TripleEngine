#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Graphics/Vertex.h"
#include "Utils/Hash.h"

namespace TripleEngineCore::Asset
{
    class Mesh {
    public:
        std::vector<Graphics::Vertex> vertices;
        std::vector<uint32_t> indices;
        uint32_t materialIndex = UINT32_MAX;

        uint64_t geometryHash = 0;

        void computeHash() {
            uint64_t h = 0;

            if (!vertices.empty()) {
                h = Hash::combine(h,
                    Hash::fnv1a(vertices.data(),
                        vertices.size() * sizeof(Graphics::Vertex)));
            }

            if (!indices.empty()) {
                h = Hash::combine(h,
                    Hash::fnv1a(indices.data(),
                        indices.size() * sizeof(uint32_t)));
            }

            geometryHash = h;
        }
    };
}

#endif // MESH_H
