#ifndef GLTFTOOL_H
#define GLTFTOOL_H

#include <tiny_gltf.h>
#include "Vec3.h"

namespace TripleEngineCore::Tool::GLTF {
    bool ReadVec3(
        const tinygltf::Model& model,
        int accessorIdx,
        std::vector<TripleMath::Vec3>& out
    );

    bool ReadVec2(
        const tinygltf::Model& model,
        int accessorIdx,
        std::vector<TripleMath::Vec2>& out
    );

    bool ReadIndices(
        const tinygltf::Model& model,
        int accessorIdx,
        std::vector<uint32_t>& out
    );
}

#endif // GLTFTOOL_H
