#include "Graphics/Model.h"

namespace TripleEngineCore::Graphics {
    Model Model::CreateCube() {
        Model cube;
        Mesh mesh;

        Vertex v;
        TripleMath::Vec3 positions[8] = {
            {-0.5f, -0.5f, -0.5f},
            { 0.5f, -0.5f, -0.5f},
            { 0.5f,  0.5f, -0.5f},
            {-0.5f,  0.5f, -0.5f},
            {-0.5f, -0.5f,  0.5f},
            { 0.5f, -0.5f,  0.5f},
            { 0.5f,  0.5f,  0.5f},
            {-0.5f,  0.5f,  0.5f}
        };

        for (int i = 0; i < 8; ++i) {
            v.position = positions[i];
            v.normal = { 0, 0, 0 };
            v.tangent = { 0, 0, 0 };
            v.bitangent = { 0, 0, 1 };
            v.color = { 1, 1, 0 };
            v.uv = { 0, 0 };
            mesh.vertices.push_back(v);
        }

        uint32_t inds[] = {
            0,1,2, 2,3,0, // -Z
            4,5,6, 6,7,4, // +Z
            0,4,7, 7,3,0, // -X
            1,5,6, 6,2,1, // +X
            3,2,6, 6,7,3, // +Y
            0,1,5, 5,4,0  // -Y
        };

        mesh.indices.insert(mesh.indices.end(), std::begin(inds), std::end(inds));

        cube.meshes.push_back(mesh);

        return cube;
    }
}
