#include "Utils/AssimpHelper.h"
#include "stb_image.h"
#include "TLogger.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace TripleEngineCore::Graphics;

namespace TripleEngineCore::Utils {
	AssimpHelper::LoadedModel AssimpHelper::LoadModel(const std::string& path)
	{
        Assimp::Importer importer;

        unsigned int flags = aiProcess_Triangulate
            | aiProcess_GenNormals
            | aiProcess_FlipUVs
            | aiProcess_CalcTangentSpace;

        const aiScene* scene = importer.ReadFile(path, flags);

        if (!scene || !scene->mRootNode) {
            TripleLogger::TLogger::ModuleError("AssimpHelper", "fail load model: ({})", path);
            return LoadedModel();
        }

        LoadedModel loadedModel;
        loadedModel.meshes.reserve(scene->mNumMeshes);

        for (unsigned m = 0; m < scene->mNumMeshes; ++m) {
            aiMesh* mesh = scene->mMeshes[m];

            LoadedMesh loadedMesh;
            loadedMesh.name = mesh->mName.C_Str();
            loadedMesh.materialIndex = mesh->mMaterialIndex;
            loadedMesh.vertices.reserve(mesh->mNumVertices);
            loadedMesh.indices.reserve(mesh->mNumFaces * 3);

            for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
                Vertex v;
                v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

                if (mesh->HasNormals())
                    v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

                if (mesh->HasTangentsAndBitangents()) {
                    v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                    v.bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
                }

                if (mesh->HasTextureCoords(0))
                    v.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

                loadedMesh.vertices.push_back(v);
            }

            for (unsigned f = 0; f < mesh->mNumFaces; ++f) {
                const aiFace& face = mesh->mFaces[f];
                for (unsigned i = 0; i < face.mNumIndices; ++i)
                    loadedMesh.indices.push_back(face.mIndices[i]);
            }

            loadedModel.meshes.push_back(std::move(loadedMesh));
        }

        return loadedModel;
	}
    AssimpHelper::LoadedTexture AssimpHelper::LoadEmbeddedTexture(aiTexture* texture)
    {
        LoadedTexture tex;

        tex.name = texture->mFilename.C_Str();

        if (texture->mHeight == 0) {
            int width, height, channels;
            unsigned char* data = stbi_load_from_memory(
                reinterpret_cast<unsigned char*>(texture->pcData),
                texture->mWidth,
                &width, &height, &channels, 4
            );

            if (data) {
                tex.width = width;
                tex.height = height;
                tex.pixels.assign(data, data + width * height * 4);
                stbi_image_free(data);
            }
        }
        else {
            tex.width = texture->mWidth;
            tex.height = texture->mHeight;
            tex.pixels.assign(
                reinterpret_cast<unsigned char*>(texture->pcData),
                reinterpret_cast<unsigned char*>(texture->pcData) + tex.width * tex.height * 4
            );
        }

        return tex;
    }
    AssimpHelper::LoadedTexture AssimpHelper::LoadFileTexture(const std::string path)
    {
        LoadedTexture tex;
        tex.name = path;
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (data) {
            tex.width = width;
            tex.height = height;
            tex.pixels.assign(data, data + width * height * 4);
            stbi_image_free(data);
        }
        return tex;
    }
    AssimpHelper::LoadedTexture AssimpHelper::LoadTexture(aiMaterial* material, uint16_t type, const aiScene* scene)
    {
        aiString texPath;
        if (material->GetTexture((aiTextureType)type, 0, &texPath) != AI_SUCCESS)
            return LoadedTexture();

        std::string pathStr = texPath.C_Str();

        if (!pathStr.empty() && pathStr[0] == '*') {
            int index = std::stoi(pathStr.substr(1));
            if (index >= 0 && index < scene->mNumTextures) {
                return LoadEmbeddedTexture(scene->mTextures[index]);
            }
        }
        else {
            return LoadFileTexture(pathStr);
        }

        return LoadedTexture();
    }
}