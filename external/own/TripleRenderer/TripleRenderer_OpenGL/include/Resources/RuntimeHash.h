#ifndef RUNTIME_HASH_H
#define RUNTIME_HASH_H

#include <cstdint>
#include <functional>

namespace TripleEngineCore::TripleRenderer::Resources
{
    struct RuntimeTextureKey {
		uint64_t contentHash;

        bool operator==(const RuntimeTextureKey& other) const {
			return contentHash == other.contentHash;
        }
    };

    struct RuntimeMeshKey
    {
        uint64_t geometryHash;

        bool operator==(const RuntimeMeshKey& other) const {
			return geometryHash == other.geometryHash;
		}
    };

    struct RuntimeShaderKey 
    {
        uint64_t vertexHash;
        uint64_t fragmentHash;

        bool operator==(const RuntimeShaderKey& other) const {
            return vertexHash == other.vertexHash && fragmentHash == other.fragmentHash;
        }
    };

    struct RuntimeTextureHasher {
        size_t operator()(const RuntimeTextureKey& key) const {
            return std::hash<uint64_t>()(key.contentHash);
        }
    };

    struct RuntimeMeshHasher {
        size_t operator()(const RuntimeMeshKey& key) const {
            return std::hash<uint64_t>()(key.geometryHash);
        }
    };

    struct RuntimeShaderHasher {
        size_t operator()(const RuntimeShaderKey& key) const {
            size_t h = std::hash<uint64_t>()(key.vertexHash);
            h ^= std::hash<uint64_t>()(key.fragmentHash) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

#endif // RUNTIME_HASH_H
