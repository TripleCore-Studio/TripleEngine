#include "triple/game/asset/AssimpHelper.h"

#include <triple/log/Logger.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/GltfMaterial.h>

#include <stb_image.h>

namespace triple::game {
	AssimpHelper::LoadedModel AssimpHelper::loadModel(const std::string &path) {
		Assimp::Importer importer;

		unsigned int flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs |
		                     aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices;

		const aiScene *scene = importer.ReadFile(path, flags);

		if (!scene || !scene->mRootNode) {
			triple::log::Logger::ModuleError("AssimpHelper", "fail load model: ({})", path);
			return LoadedModel();
		}

		LoadedModel loadedModel;
		loadGeometry(scene, loadedModel);
		loadMaterials(scene, loadedModel);

		return loadedModel;
	}
	AssimpHelper::LoadedTexture AssimpHelper::loadEmbeddedTexture(const aiTexture *texture) {
		LoadedTexture tex;

		tex.name = texture->mFilename.C_Str();

		if (texture->mHeight == 0) {
			int width, height, channels;
			unsigned char *data =
			    stbi_load_from_memory(reinterpret_cast<unsigned char *>(texture->pcData),
			                          texture->mWidth, &width, &height, &channels, 4);

			if (data) {
				tex.width = width;
				tex.height = height;
				tex.channels = channels;
				tex.pixels.assign(data, data + width * height * 4);
				stbi_image_free(data);
			}
		} else {
			tex.width = texture->mWidth;
			tex.height = texture->mHeight;
			tex.channels = 4;
			tex.pixels.assign(reinterpret_cast<unsigned char *>(texture->pcData),
			                  reinterpret_cast<unsigned char *>(texture->pcData) +
			                      tex.width * tex.height * 4);
		}

		return tex;
	}
	AssimpHelper::LoadedTexture AssimpHelper::loadFileTexture(const std::string &path) {
		LoadedTexture tex;
		tex.name = path;
		int width, height, channels;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);
		if (data) {
			tex.width = width;
			tex.height = height;
			tex.channels = channels;
			tex.pixels.assign(data, data + width * height * 4);
			stbi_image_free(data);
		}
		return tex;
	}
	AssimpHelper::LoadedTexture AssimpHelper::loadTexture(const aiMaterial *material, uint16_t type,
	                                                      const aiScene *scene) {
		aiString texPath;
		if (material->GetTexture((aiTextureType)type, 0, &texPath) != AI_SUCCESS)
			return LoadedTexture();

		std::string pathStr = texPath.C_Str();

		if (!pathStr.empty() && pathStr[0] == '*') {
			int index = std::stoi(pathStr.substr(1));
			if (index >= 0 && index < scene->mNumTextures) {
				return loadEmbeddedTexture(scene->mTextures[index]);
			}
		} else {
			return loadFileTexture(pathStr);
		}

		return LoadedTexture();
	}
	static float getUnitScaleToMeters(const aiScene *scene) {
		if (scene->mMetaData) {
			double unitScaleFactor = 0.0;
			// FBX stores this as centimeters per unit, e.g. 1.0 = 1cm/unit, 100.0 = 1m/unit
			if (scene->mMetaData->Get("UnitScaleFactor", unitScaleFactor) && unitScaleFactor > 0.0)
				return static_cast<float>(unitScaleFactor / 100.0);
		}
		return 1.0f;
	}

	void AssimpHelper::loadGeometry(const ::aiScene *scene, LoadedModel &outModel) {
		outModel.meshes.reserve(scene->mNumMeshes);

		const float kScale = getUnitScaleToMeters(scene);

		for (unsigned m = 0; m < scene->mNumMeshes; ++m) {
			aiMesh *mesh = scene->mMeshes[m];

			LoadedMesh loadedMesh;
			loadedMesh.name = mesh->mName.C_Str();
			loadedMesh.materialIndex = mesh->mMaterialIndex;
			loadedMesh.vertices.reserve(mesh->mNumVertices);
			loadedMesh.indices.reserve(mesh->mNumFaces * 3);

			for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
				gfx::Vertex v;
				v.position = {mesh->mVertices[i].x * kScale, mesh->mVertices[i].y * kScale,
				              mesh->mVertices[i].z * kScale};

				if (mesh->HasNormals())
					v.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

				if (mesh->HasTangentsAndBitangents()) {
					v.tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
					v.bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y,
					               mesh->mBitangents[i].z};
				}

				if (mesh->HasTextureCoords(0))
					v.uv = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

				loadedMesh.vertices.push_back(v);
			}

			for (unsigned f = 0; f < mesh->mNumFaces; ++f) {
				const aiFace &face = mesh->mFaces[f];
				for (unsigned i = 0; i < face.mNumIndices; ++i)
					loadedMesh.indices.push_back(face.mIndices[i]);
			}

			outModel.meshes.push_back(std::move(loadedMesh));
		}
	}
	void AssimpHelper::loadMaterials(const ::aiScene *scene, LoadedModel &outModel) {
		outModel.materials.reserve(scene->mNumMaterials);

		for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
			aiMaterial *mat = scene->mMaterials[i];

			LoadedMaterial loadedMat;

			aiString name;
			if (mat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
				loadedMat.name = name.C_Str();

			float opacity = 1.0f;
			mat->Get(AI_MATKEY_OPACITY, opacity);
			loadedMat.opacity = opacity;

			aiString alphaModeStr;
			if (mat->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaModeStr) == AI_SUCCESS) {
				std::string mode = alphaModeStr.C_Str();
				if (mode == "BLEND") {
					loadedMat.blendMode = MaterialBlendMode::Transparent;
				} else if (mode == "MASK") {
					loadedMat.blendMode = MaterialBlendMode::AlphaCutoff;
				} else {
					loadedMat.blendMode = MaterialBlendMode::Opaque;
				}
			} else {
				loadedMat.blendMode =
				    (opacity < 1.0f) ? MaterialBlendMode::Transparent : MaterialBlendMode::Opaque;
			}

			LoadedTexture diffuse = loadTexture(mat, aiTextureType_DIFFUSE, scene);
			if (!diffuse.pixels.empty())
				loadedMat.diffuseTextures.push_back(std::move(diffuse));

			LoadedTexture normal = loadTexture(mat, aiTextureType_NORMALS, scene);
			if (normal.pixels.empty()) {
				normal = loadTexture(mat, aiTextureType_HEIGHT, scene);
			}
			if (!normal.pixels.empty())
				loadedMat.normalTextures.push_back(std::move(normal));

			outModel.materials.push_back(std::move(loadedMat));
		}
	}
} // namespace triple::game