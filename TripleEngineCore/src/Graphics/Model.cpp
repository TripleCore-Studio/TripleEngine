#include "Asset/Model.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <memory>

namespace TripleEngineCore::Graphics {

    struct ObjIndex {
        uint32_t pos;

        bool operator==(const ObjIndex& other) const {
            return pos == other.pos;
        }
    };

}

namespace std {
    template<>
    struct hash<TripleEngineCore::Graphics::ObjIndex> {
        size_t operator()(const TripleEngineCore::Graphics::ObjIndex& v) const {
            return hash<uint32_t>()(v.pos);
        }
    };
}

namespace TripleEngineCore::Asset {
    Model Model::CreateCube() {
        Model cube;

        TripleMath::Vec3 normals[6] = {
            { 0,  0, -1}, // -Z
            { 0,  0,  1}, // +Z
            {-1,  0,  0}, // -X
            { 1,  0,  0}, // +X
            { 0,  1,  0}, // +Y
            { 0, -1,  0}, // -Y
        };

        TripleMath::Vec3 positions[8] = {
            {-0.5f, -0.5f, -0.5f}, // 0
            { 0.5f, -0.5f, -0.5f}, // 1
            { 0.5f,  0.5f, -0.5f}, // 2
            {-0.5f,  0.5f, -0.5f}, // 3
            {-0.5f, -0.5f,  0.5f}, // 4
            { 0.5f, -0.5f,  0.5f}, // 5
            { 0.5f,  0.5f,  0.5f}, // 6
            {-0.5f,  0.5f,  0.5f}, // 7
        };

        struct Face {
            int idx[4];
            TripleMath::Vec3 normal;
            bool invertU;
        };

        Face faces[6] = {
            {{0,1,2,3}, normals[0], true},  // -Z
            {{5,4,7,6}, normals[1], true},  // +Z
            {{4,0,3,7}, normals[2], false}, // -X
            {{1,5,6,2}, normals[3], false}, // +X
            {{3,2,6,7}, normals[4], false}, // +Y
            {{4,5,1,0}, normals[5], false}, // -Y
        };

        Mesh mesh;
        for (auto& face : faces) {
            Graphics::Vertex v[4];
            float u0 = face.invertU ? 1.0f : 0.0f;
            float u1 = face.invertU ? 0.0f : 1.0f;

            v[0].position = positions[face.idx[0]]; v[0].normal = face.normal; v[0].uv = { u0, 0.0f };
            v[1].position = positions[face.idx[1]]; v[1].normal = face.normal; v[1].uv = { u1, 0.0f };
            v[2].position = positions[face.idx[2]]; v[2].normal = face.normal; v[2].uv = { u1, 1.0f };
            v[3].position = positions[face.idx[3]]; v[3].normal = face.normal; v[3].uv = { u0, 1.0f };

            uint32_t base = static_cast<uint32_t>(mesh.vertices.size());
            for (int i = 0; i < 4; i++) mesh.vertices.push_back(v[i]);

            mesh.indices.push_back(base + 0);
            mesh.indices.push_back(base + 1);
            mesh.indices.push_back(base + 2);

            mesh.indices.push_back(base + 2);
            mesh.indices.push_back(base + 3);
            mesh.indices.push_back(base + 0);
        }

        cube.meshes.push_back(std::move(mesh));
        return cube;
    }

}
