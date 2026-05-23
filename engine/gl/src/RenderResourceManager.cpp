#include "triple/gl/RenderResourceManager.h"
#include "triple/log/Logger.h"
#include <glad/glad.h>

namespace triple::gl {
	GLGeometry *RenderResourceManager::getGLGeometry(const gfx::GPUHandle geometry) {
		auto it = m_primitiveCache.find(geometry);
		if (it != m_primitiveCache.end())
			return &it->second;
		return nullptr;
	}

	gfx::GPUHandle RenderResourceManager::createGLGeometry(const gfx::GeometryDesc &desc) {
		GLGeometry geom;
		gfx::GPUHandle handle = m_nextHandle++;
		geom.vbo.setData(desc.vertices, desc.vertexCount * sizeof(gfx::Vertex),
		                 VertexBufferObject::Usage::STATIC_DRAW);
		geom.ibo.setData(desc.indices, desc.indexCount * sizeof(uint32_t),
		                 IndexBufferObject::Usage::STATIC_DRAW);
		geom.vao.setData(geom.vbo);
		geom.vao.setIndexData(geom.ibo);
		m_primitiveCache.emplace(handle, std::move(geom));
		return handle;
	}

	GLTexture *RenderResourceManager::getGLTexture(gfx::GPUHandle texture) {
		auto it = m_textureCache.find(texture);
		if (it != m_textureCache.end())
			return &it->second;
		return nullptr;
	}

	gfx::GPUHandle RenderResourceManager::createGLTexture(const gfx::TextureDesc &desc) {
		GLTexture texGPU;
		gfx::GPUHandle handle = m_nextHandle++;
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
		glTexImage2D(texGPU.target, 0, texGPU.internalFormat, texGPU.width, texGPU.height, 0,
		             texGPU.format, texGPU.type, desc.data);

		glTexParameteri(texGPU.target, GL_TEXTURE_MIN_FILTER, texGPU.minFilter);
		glTexParameteri(texGPU.target, GL_TEXTURE_MAG_FILTER, texGPU.magFilter);
		glTexParameteri(texGPU.target, GL_TEXTURE_WRAP_S, texGPU.wrapU);
		glTexParameteri(texGPU.target, GL_TEXTURE_WRAP_T, texGPU.wrapV);

		glBindTexture(texGPU.target, 0);
		m_textureCache.emplace(handle, std::move(texGPU));
		return handle;
	}

	GLShader *RenderResourceManager::getGLShader(gfx::GPUHandle shader) {
		auto it = m_shaderCache.find(shader);
		if (it != m_shaderCache.end())
			return &it->second;
		return nullptr;
	}

	gfx::GPUHandle RenderResourceManager::createGLShader(const gfx::ShaderDesc &desc) {
		GLShader shader;
		gfx::GPUHandle handle = m_nextHandle++;

		if (!shader.compileProgram(desc.vCode, desc.fCode)) {
			triple::log::Logger::ModuleCritical("OpenGL -> RenderResourceManager",
			                                    "Failed to compile shader program: " +
			                                        shader.getErrorLog());
			return gfx::INVALID_GPU_HANDLE;
		}

		m_shaderCache.emplace(handle, std::move(shader));

		return handle;
	}

	void RenderResourceManager::bindMaterial(gfx::RenderMaterial &material) {
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
} // namespace triple::gl