#include "triple/gfx/GfxDescs.h"
#include "triple/gl/OpenGLRenderer.h"

#include <assert.h>

#include "triple/gl/utils/TextureUtils.h"

namespace triple::gl {
	GLint componentCount(VertexAttribType type) {
		switch (type) {
			case VertexAttribType::Float:
				return 1;
			case VertexAttribType::Vec2:
				return 2;
			case VertexAttribType::Vec3:
				return 3;
			case VertexAttribType::Vec4:
				return 4;
		}
		return 1;
	}

	GLenum glBaseType(VertexAttribType /*type*/) { return GL_FLOAT; }

	[[nodiscard]] TextureHandle OpenGLRenderer::uploadTexture(const TextureDesc &desc) {
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		GLTextureFormatInfo info = toGLFormat(desc.format);
		glTexImage2D(GL_TEXTURE_2D, 0, info.format, desc.width, desc.height, 0, info.format,
		             info.type, desc.pixels);
		if (desc.generateMips)
			glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		GpuHandle raw = m_resources->pool<GLTextureRes>(ResourceType::Texture)
		                    .create(GLTextureRes{id, GL_TEXTURE_2D, desc.width, desc.height});

		return TextureHandle{raw};
	}

	[[nodiscard]] ShaderHandle OpenGLRenderer::uploadShader(const ShaderDesc &desc) {
		GLuint vert = compileStage(GL_VERTEX_SHADER, desc.vertexSource.c_str());
		if (vert == 0)
			return ShaderHandle{};
		GLuint frag = compileStage(GL_FRAGMENT_SHADER, desc.fragmentSource.c_str());
		if (frag == 0) {
			glDeleteShader(vert);
			return ShaderHandle{};
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, vert);
		glAttachShader(program, frag);
		glLinkProgram(program);

		glDeleteShader(vert);
		glDeleteShader(frag);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (!linked) {
			char log[1024];
			glGetProgramInfoLog(program, sizeof(log), nullptr, log);
			setLastError(std::string("uploadShader: link failed: ") + log);
			glDeleteProgram(program);
			return ShaderHandle{};
		}

		GLint modelLoc = glGetUniformLocation(program, "uModel");
		GLint viewLoc = glGetUniformLocation(program, "uView");
		GLint projLoc = glGetUniformLocation(program, "uProjection");

		GpuHandle raw =
		    m_resources->pool<GLShaderRes>(ResourceType::Shader)
		        .create(GLShaderRes{program, desc.uniformBlockSize, modelLoc, viewLoc, projLoc});
		return ShaderHandle{raw};
	}

	[[nodiscard]] GeometryHandle OpenGLRenderer::uploadGeometry(const GeometryDesc &desc) {
		GLuint vao, vbo, ebo;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(desc.vertexCount) * desc.vertexStride,
		             desc.vertexData, GL_STATIC_DRAW);

		for (const auto &attr : desc.layout.attributes) {
			glEnableVertexAttribArray(attr.location);
			glVertexAttribPointer(attr.location, componentCount(attr.type), glBaseType(attr.type),
			                      GL_FALSE, static_cast<GLsizei>(desc.layout.stride),
			                      reinterpret_cast<void *>(static_cast<uintptr_t>(attr.offset)));
		}

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		             static_cast<GLsizeiptr>(desc.indexCount) * sizeof(uint32_t), desc.indices,
		             GL_STATIC_DRAW);

		glBindVertexArray(0);

		GpuHandle raw = m_resources->pool<GLGeometryRes>(ResourceType::Geometry)
		                    .create(GLGeometryRes{vao, vbo, ebo, desc.indexCount, GL_UNSIGNED_INT});
		return GeometryHandle{raw};
	}

	void OpenGLRenderer::unloadTexture(TextureHandle handle) {
		auto &pool = m_resources->pool<GLTextureRes>(ResourceType::Texture);
		auto *res = pool.get(handle.raw);
		if (!res) {
			setLastError("unloadTexture: invalid or stale handle");
			return;
		}
		glDeleteTextures(1, &res->id);
		pool.destroy(handle.raw);
	}

	void OpenGLRenderer::unloadShader(ShaderHandle handle) {
		auto &pool = m_resources->pool<GLShaderRes>(ResourceType::Shader);
		auto *res = pool.get(handle.raw);
		if (!res) {
			setLastError("unloadShader: invalid or stale handle");
			return;
		}
		glDeleteProgram(res->program);
		pool.destroy(handle.raw);
	}

	void OpenGLRenderer::unloadGeometry(GeometryHandle handle) {
		auto &pool = m_resources->pool<GLGeometryRes>(ResourceType::Geometry);
		auto *res = pool.get(handle.raw);
		if (!res) {
			setLastError("unloadGeometry: invalid or stale handle");
			return;
		}
		glDeleteVertexArrays(1, &res->vao);
		glDeleteBuffers(1, &res->vbo);
		glDeleteBuffers(1, &res->ebo);
		pool.destroy(handle.raw);
	}

	[[nodiscard]] RenderTargetHandle
	OpenGLRenderer::createRenderTarget(const RenderTargetDesc &desc) {
		if (desc.type == RenderTargetType::BackBuffer) {
			setLastError("createRenderTarget: cannot create a BackBuffer target via public API");

			assert(
			    false &&
			    "createRenderTarget: BackBuffer target must be created internally by the renderer");

			return RenderTargetHandle{};
		}

		GLRenderTargetRes res;
		res.width = desc.width;
		res.height = desc.height;
		res.type = gfx::RenderTargetType::Texture;

		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		res.fbo = fbo;

		std::vector<GLenum> drawBuffers;
		for (size_t i = 0; i < desc.colorFormats.size(); ++i) {
			gfx::TextureHandle tex = m_resources->createColorAttachmentTexture(
			    desc.width, desc.height, desc.colorFormats[i]);

			res.colorTextures.push_back(tex);
			GLuint glTex = m_resources->pool<GLTextureRes>(ResourceType::Texture).get(tex.raw)->id;

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, glTex,
			                       0);

			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		if (desc.hasDepth) {
			gfx::TextureHandle depthTex = m_resources->createDepthAttachmentTexture(
			    desc.width, desc.height, desc.depthFormat);

			res.depthTexture = depthTex;
			GLuint glDepth =
			    m_resources->pool<GLTextureRes>(ResourceType::Texture).get(depthTex.raw)->id;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glDepth, 0);
		}

		if (drawBuffers.empty()) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		} else {
			glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
		}

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			setLastError("createRenderTarget: framebuffer incomplete, status=" +
			             std::to_string(status));
			assert(false && "createRenderTarget: incomplete framebuffer");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return gfx::RenderTargetHandle{
		    m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget)
		        .create(std::move(res))};
	}

	void OpenGLRenderer::destroyRenderTarget(RenderTargetHandle handle) {
		auto &pool = m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget);
		GLRenderTargetRes *res = pool.get(handle.raw);
		if (!res) {
			setLastError("destroyRenderTarget: invalid handle");
			assert(false && "destroyRenderTarget: invalid handle");
			return;
		}

		if (res->type == gfx::RenderTargetType::BackBuffer) {
			setLastError("destroyRenderTarget: attempt to destroy the backbuffer target");
			assert(false && "destroyRenderTarget: backbuffer target must not be destroyed");
			return;
		}

		for (gfx::TextureHandle colorTex : res->colorTextures) {
			unloadTexture(colorTex);
		}
		if (res->depthTexture.isValid()) {
			unloadTexture(res->depthTexture);
		}

		glDeleteFramebuffers(1, &res->fbo);

		pool.destroy(handle.raw);
	}

	[[nodiscard]] TextureHandle OpenGLRenderer::getRenderTargetTexture(RenderTargetHandle handle,
	                                                                   uint32_t colorIndex) const {
		const GLRenderTargetRes *res =
		    m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget).get(handle.raw);
		assert(res && "getRenderTargetTexture: invalid handle");
		assert(colorIndex < res->colorTextures.size() &&
		       "getRenderTargetTexture: colorIndex out of range");
		return res->colorTextures[colorIndex];
	}

	[[nodiscard]] TextureHandle
	OpenGLRenderer::getRenderTargetDepthTexture(RenderTargetHandle handle) const {
		const GLRenderTargetRes *res =
		    m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget).get(handle.raw);
		assert(res && "getRenderTargetDepthTexture: invalid handle");
		assert(res->depthTexture.isValid() && "getRenderTargetDepthTexture: target has no depth");
		return res->depthTexture;
	}

	[[nodiscard]] ViewHandle OpenGLRenderer::createView(const ViewDesc &desc) {
		GLViewRes res{desc};

		res.activeDrawBuffers = computeDrawBuffers(desc.activeColorAttachments);

		GpuHandle raw = m_resources->pool<GLViewRes>(ResourceType::View).create(std::move(res));
		return ViewHandle{raw};
	}

	void OpenGLRenderer::destroyView(ViewHandle handle) {
		auto &pool = m_resources->pool<GLViewRes>(ResourceType::View);
		if (!pool.destroy(handle.raw))
			setLastError("destroyView: invalid or stale handle");
	}

	void OpenGLRenderer::updateView(ViewHandle handle, const ViewDesc &desc) {
		auto &pool = m_resources->pool<GLViewRes>(ResourceType::View);
		auto *res = pool.get(handle.raw);
		if (!res) {
			setLastError("updateView: invalid or stale handle");
			return;
		}
		res->desc = desc;
		res->activeDrawBuffers = computeDrawBuffers(desc.activeColorAttachments);
	}

	[[nodiscard]] RenderTargetHandle OpenGLRenderer::getBackBufferTarget() const {
		return m_backBufferTarget;
	}

	std::vector<GLenum>
	OpenGLRenderer::computeDrawBuffers(const std::vector<uint32_t> &activeColorAttachments) {
		std::vector<GLenum> result;
		result.reserve(activeColorAttachments.size());
		for (uint32_t attachmentIndex : activeColorAttachments) {
			result.push_back(GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(attachmentIndex));
		}
		return result;
	}
} // namespace triple::gl