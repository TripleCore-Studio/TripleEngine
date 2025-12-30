#include "Resources/RenderResourceManager.h"
#include "TLogger.h"
#include <glad/glad.h>

namespace TripleRenderer::GLRenderer::Resources
{
	GLGeometry* RenderResourceManager::getGLGeometry(const tec::GPUHandle geometry)
	{
		auto it = _primitiveCache.find(geometry);
		if (it != _primitiveCache.end())
			return &it->second;
		return nullptr;
	}

	tec::GPUHandle RenderResourceManager::createGLGeometry(const tec::Graphics::GeometryDesc& desc) {
		GLGeometry geom;
		tec::GPUHandle handle = _nextHandle++;
		geom.vbo.setData(desc.vertices, desc.vertexCount * sizeof(tec::Graphics::Vertex), Buffer::VertexBufferObject::Usage::STATIC_DRAW);
		geom.ibo.setData(desc.indices, desc.indexCount * sizeof(uint32_t), Buffer::IndexBufferObject::Usage::STATIC_DRAW);
		geom.vao.setData(geom.vbo);
		geom.vao.setIndexData(geom.ibo);
		_primitiveCache.emplace(handle, std::move(geom));
		return handle;
	}

	GLTexture* Resources::RenderResourceManager::getGLTexture(tec::GPUHandle texture)
	{
		auto it = _textureCache.find(texture);
		if (it != _textureCache.end())
			return &it->second;
		return nullptr;
	}

	tec::GPUHandle RenderResourceManager::createGLTexture(const tec::Graphics::TextureDesc& desc) {
		GLTexture texGPU;
		tec::GPUHandle handle = _nextHandle++;
		glGenTextures(1, &texGPU.id);
		texGPU.target = GL_TEXTURE_2D;
		texGPU.width = desc.width;
		texGPU.height = desc.height;
		texGPU.mipLevels = 1;
		texGPU.internalFormat = GL_RGBA8;
		texGPU.format = GL_RGBA;
		texGPU.type = GL_UNSIGNED_BYTE;

		texGPU.minFilter = GL_LINEAR;
		texGPU.magFilter = GL_LINEAR;
		texGPU.wrapU = GL_REPEAT;
		texGPU.wrapV = GL_REPEAT;
		texGPU.wrapW = GL_REPEAT;
		texGPU.hasMipmaps = false;

		glBindTexture(texGPU.target, texGPU.id);
		glTexImage2D(texGPU.target, 0, texGPU.internalFormat,
			texGPU.width, texGPU.height, 0,
			texGPU.format, texGPU.type,
			desc.data);

		glTexParameteri(texGPU.target, GL_TEXTURE_MIN_FILTER, texGPU.minFilter);
		glTexParameteri(texGPU.target, GL_TEXTURE_MAG_FILTER, texGPU.magFilter);
		glTexParameteri(texGPU.target, GL_TEXTURE_WRAP_S, texGPU.wrapU);
		glTexParameteri(texGPU.target, GL_TEXTURE_WRAP_T, texGPU.wrapV);

		glBindTexture(texGPU.target, 0);
		_textureCache.emplace(handle, std::move(texGPU));
		return handle;
	}

	GLShader* RenderResourceManager::getGLShader(tec::GPUHandle shader)
	{
		auto it = _shaderCache.find(shader);
		if (it != _shaderCache.end())
			return &it->second;
		return nullptr;
	}

	tec::GPUHandle RenderResourceManager::createGLShader(const tec::Graphics::ShaderDesc& desc) {
		GLShader shader;
		tec::GPUHandle handle = _nextHandle++;

		if (!shader.compileProgram(desc.vCode, desc.fCode))
		{
			TripleLogger::TLogger::ModuleError("OpenGL -> RenderResourceManager", "Failed to compile shader program: " + shader.getErrorLog());
			return tec::INVALID_GPU_HANDLE;
		}

		_shaderCache.emplace(handle, std::move(shader));

		return handle;
	}

	void RenderResourceManager::bindMaterial(tec::Graphics::RenderMaterial& material)
	{
		/*ShaderProgram* program = getShaderProgram(material.shader);
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
		program->setUniform1f("u_Roughness", material.roughness);*/
	}
}