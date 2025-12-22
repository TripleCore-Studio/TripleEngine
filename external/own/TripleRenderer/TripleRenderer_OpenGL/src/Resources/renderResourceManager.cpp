#include "Resources/RenderResourceManager.h"
#include "TLogger.h"
#include <glad/glad.h>

namespace TripleEngineCore::TripleRenderer::Resources
{
	MeshGPU* Resources::RenderResourceManager::getMeshGPU(const Runtime::RuntimeMesh& mesh)
	{
		RuntimeMeshKey key; key.geometryHash = mesh.geometryHash;

		auto it = meshCache.find(key);
		if (it != meshCache.end())
			return it->second.get();

		auto gpuMesh = std::make_unique<MeshGPU>();

		gpuMesh->vbo.setData(
			static_cast<const void*>(mesh.vertices),
			mesh.vertexCount * sizeof(TripleEngineCore::Graphics::Vertex),
			VertexBufferObject::Usage::STATIC_DRAW);

		gpuMesh->ibo.setData(
			static_cast<const void*>(mesh.indices),
			mesh.indexCount * sizeof(uint32_t),
			IndexBufferObject::Usage::STATIC_DRAW);

		gpuMesh->vao.setData(gpuMesh->vbo);
		gpuMesh->vao.setIndexData(gpuMesh->ibo);

		auto ptr = gpuMesh.get();
		meshCache[key] = std::move(gpuMesh);

		return ptr;
	}
	TextureGPU* Resources::RenderResourceManager::getTextureGPU(const Runtime::RuntimeTexture& texture)
	{
		RuntimeTextureKey key; key.contentHash = texture.contentHash;

		auto it = textureCache.find(key);
		if (it != textureCache.end())
			return it->second.get();

		auto texGPU = std::make_unique<TextureGPU>();
		glGenTextures(1, &texGPU->id);
		texGPU->target = GL_TEXTURE_2D;
		texGPU->width = texture.width;
		texGPU->height = texture.height;
		texGPU->mipLevels = 1;
		texGPU->internalFormat = GL_RGBA8;
		texGPU->format = GL_RGBA;
		texGPU->type = GL_UNSIGNED_BYTE;

		texGPU->minFilter = GL_LINEAR;
		texGPU->magFilter = GL_LINEAR;
		texGPU->wrapU = GL_REPEAT;
		texGPU->wrapV = GL_REPEAT;
		texGPU->wrapW = GL_REPEAT;
		texGPU->hasMipmaps = false;

		glBindTexture(texGPU->target, texGPU->id);
		glTexImage2D(texGPU->target, 0, texGPU->internalFormat,
			texGPU->width, texGPU->height, 0,
			texGPU->format, texGPU->type,
			texture.data);

		glTexParameteri(texGPU->target, GL_TEXTURE_MIN_FILTER, texGPU->minFilter);
		glTexParameteri(texGPU->target, GL_TEXTURE_MAG_FILTER, texGPU->magFilter);
		glTexParameteri(texGPU->target, GL_TEXTURE_WRAP_S, texGPU->wrapU);
		glTexParameteri(texGPU->target, GL_TEXTURE_WRAP_T, texGPU->wrapV);

		glBindTexture(texGPU->target, 0);

		auto ptr = texGPU.get();
		textureCache[key] = std::move(texGPU);

		return ptr;
	}
	TextureID Resources::RenderResourceManager::getTextureID(const Runtime::RuntimeTexture& texture)
	{
		const TextureGPU* gpu = getTextureGPU(texture);
		return gpu ? gpu->id : INVALID_TEXTURE_ID;
	}
	ShaderProgram* Resources::RenderResourceManager::getShaderProgram(const Runtime::RuntimeShader& shader)
	{
		RuntimeShaderKey key; key.fragmentHash = shader.fragmentHash; key.vertexHash = shader.vertexHash;
		auto it = shaderCache.find(key);
		if (it != shaderCache.end())
			return it->second.get();

		auto shaderProgram = std::make_unique<ShaderProgram>();

		if (!shaderProgram->compileProgram(shader.vertexShaderCode, shader.fragmentShaderCode))
		{
			TripleLogger::TLogger::ModuleError("OpenGL -> RenderResourceManager", "Failed to compile shader program: " + shaderProgram->getErrorLog());
			return nullptr;
		}

		auto ptr = shaderProgram.get();
		shaderCache[key] = std::move(shaderProgram);

		return ptr;
	}
	void Resources::RenderResourceManager::bindMaterial(const Runtime::RuntimeMaterial& material)
	{
		ShaderProgram* program = getShaderProgram(material.shader);
		if (!program) return;

		program->use();

		TextureGPU* albedo = getTextureGPU(material.albedoTexture);
		TextureGPU* normal = getTextureGPU(material.normalTexture);
		TextureGPU* metallic = getTextureGPU(material.metallicTexture);
		TextureGPU* roughness = getTextureGPU(material.roughnessTexture);

		if (albedo)   program->setTexture("uAlbedoTex", albedo->id, 0);
		if (normal)   program->setTexture("uNormalTex", normal->id, 1);
		if (metallic) program->setTexture("uMetallicTex", metallic->id, 2);
		if (roughness)program->setTexture("uRoughnessTex", roughness->id, 3);

		program->setUniform3fv("u_AlbedoColor", material.albedoColor.data());
		program->setUniform1f("u_Metallic", material.metallic);
		program->setUniform1f("u_Roughness", material.roughness);
	}
}