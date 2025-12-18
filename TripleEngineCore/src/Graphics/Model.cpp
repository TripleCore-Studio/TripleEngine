#include "Graphics/Model.h"
#include <fstream>
#include <sstream>
#include <unordered_map>

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

namespace TripleEngineCore::Graphics {
    Model Model::CreateCube() {
        Model cube;
		std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

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
            mesh->vertices.push_back(v);
        }

        uint32_t inds[] = {
            0,1,2, 2,3,0, // -Z
            4,5,6, 6,7,4, // +Z
            0,4,7, 7,3,0, // -X
            1,5,6, 6,2,1, // +X
            3,2,6, 6,7,3, // +Y
            0,1,5, 5,4,0  // -Y
        };

        mesh->indices.insert(mesh->indices.end(), std::begin(inds), std::end(inds));

        cube.meshes.push_back(std::move(mesh));

        return cube;
    }
    Model Model::LoadOBJ(const std::string& path) {
        Model model;
		std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

        std::vector<TripleMath::Vec3> positions;
        std::unordered_map<ObjIndex, uint32_t> indexMap;

        std::vector<TripleMath::Vec3> palette = {
            {0.9f, 0.3f, 0.3f}, 
            {0.3f, 0.9f, 0.3f}, 
            {0.3f, 0.6f, 0.9f}, 
            {0.9f, 0.9f, 0.3f}, 
            {0.7f, 0.4f, 0.9f}, 
            {0.3f, 0.9f, 0.9f} 
        };

        size_t colorIndex = 0;
        TripleMath::Vec3 currentColor = palette[0];

        std::ifstream file(path);
        if (!file.is_open())
            return model;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string type;
            ss >> type;

            if (type == "o") {
                currentColor = palette[colorIndex % palette.size()];
                colorIndex++;
            }

            else if (type == "v") {
                TripleMath::Vec3 p;
                ss >> p.x >> p.y >> p.z;
                positions.push_back(p);
            }

            else if (type == "f") {
                std::string token;
                std::vector<uint32_t> faceIndices;

                while (ss >> token) {
                    std::stringstream ts(token);
                    std::string posStr;
                    std::getline(ts, posStr, '/');

                    uint32_t posIndex = std::stoi(posStr) - 1;
                    ObjIndex key{ posIndex };

                    auto it = indexMap.find(key);
                    if (it != indexMap.end()) {
                        faceIndices.push_back(it->second);
                    }
                    else {
                        Vertex v{};
                        v.position = positions[posIndex];
                        v.normal = { 0, 0, 0 };
                        v.tangent = { 0, 0, 0 };
                        v.bitangent = { 0, 0, 1 };
                        v.color = currentColor;
                        v.uv = { 0, 0 };

                        uint32_t newIndex = (uint32_t)mesh->vertices.size();
                        mesh->vertices.push_back(v);
                        indexMap[key] = newIndex;
                        faceIndices.push_back(newIndex);
                    }
                }

                if (faceIndices.size() == 3) {
                    mesh->indices.insert(mesh->indices.end(), {
                        faceIndices[0], faceIndices[1], faceIndices[2]
                        });
                }
                else if (faceIndices.size() == 4) {
                    mesh->indices.insert(mesh->indices.end(), {
                        faceIndices[0], faceIndices[1], faceIndices[2],
                        faceIndices[0], faceIndices[2], faceIndices[3]
                        });
                }
            }
        }

        model.meshes.push_back(std::move(mesh));
        return model;
    }
}
