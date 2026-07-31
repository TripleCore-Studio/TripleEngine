#include "triple/gl/OpenGLRenderer.h"

#include <assert.h>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <triple/log/Logger.h>

#include "triple/gl/utils/TextureUtils.h"

namespace triple::gl {
	GLuint OpenGLRenderer::compileStage(GLenum stage, const char *src) {
		GLuint id = glCreateShader(stage);
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		GLint ok;
		glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
		if (!ok) {
			char log[1024];
			glGetShaderInfoLog(id, sizeof(log), nullptr, log);
			setLastError(std::string("shader compile failed: ") + log);
			glDeleteShader(id);
			return 0;
		}
		return id;
	}
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
} // namespace triple::gl

namespace triple::gl {
	OpenGLRenderer::OpenGLRenderer() { m_resources = std::make_unique<ResourceManager>(); }

	bool OpenGLRenderer::initialize(const RendererConfig &config) {
		GLFWwindow *window = static_cast<GLFWwindow *>(config.windowHandle);
		m_window = window;
		glfwMakeContextCurrent(window);

		if (config.vsync)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			triple::log::Logger::moduleError("OpenGLRenderer", "Error initialize glad");
			return false;
		}

		glViewport(0, 0, config.width, config.height);

		RenderTargetDesc backBufferDesc{};
		backBufferDesc.type = RenderTargetType::BackBuffer;
		backBufferDesc.width = config.width;
		backBufferDesc.height = config.height;
		m_backBufferTarget = createRenderTarget(backBufferDesc);

		glGenBuffers(1, &m_materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, m_materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, gfx::kMaxUniformBlockSize, nullptr,
		             GL_DYNAMIC_DRAW); // с запасом, под самый большой материал
		glBindBufferBase(GL_UNIFORM_BUFFER, kMaterialUboBinding, m_materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}
	void OpenGLRenderer::shutdown() {
		// textures
		m_resources->pool<GLTextureRes>(ResourceType::Texture).forEachAlive([](GLTextureRes &res) {
			glDeleteTextures(1, &res.id);
		});

		// shaders
		m_resources->pool<GLShaderRes>(ResourceType::Shader).forEachAlive([](GLShaderRes &res) {
			glDeleteProgram(res.program);
		});

		// geometry
		m_resources->pool<GLGeometryRes>(ResourceType::Geometry)
		    .forEachAlive([](GLGeometryRes &res) {
			    glDeleteVertexArrays(1, &res.vao);
			    glDeleteBuffers(1, &res.vbo);
			    glDeleteBuffers(1, &res.ebo);
		    });

		// render targets (fbo; their textures will already be removed above, since they are also in
		// the Texture-pool
		m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget)
		    .forEachAlive([](GLRenderTargetRes &res) {
			    if (!res.isBackBuffer)
				    glDeleteFramebuffers(1, &res.fbo);
		    });

		// views do not own GL resources directly - just data, there is nothing to clean.

		m_window = nullptr;
	}
	void OpenGLRenderer::resize(uint32_t width, uint32_t height) {
		glViewport(0, 0, width, height);

		auto &pool = m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget);
		auto *backBufferRes = pool.get(m_backBufferTarget.raw);
		if (backBufferRes) {
			backBufferRes->width = width;
			backBufferRes->height = height;
		}
	}

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
	};
	void OpenGLRenderer::unloadShader(ShaderHandle handle) {
		auto &pool = m_resources->pool<GLShaderRes>(ResourceType::Shader);
		auto *res = pool.get(handle.raw);
		if (!res) {
			setLastError("unloadShader: invalid or stale handle");
			return;
		}
		glDeleteProgram(res->program);
		pool.destroy(handle.raw);
	};
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
	};

	[[nodiscard]] RenderTargetHandle
	OpenGLRenderer::createRenderTarget(const RenderTargetDesc &desc) {
		if (desc.type == RenderTargetType::BackBuffer) {
			GpuHandle raw =
			    m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget)
			        .create(GLRenderTargetRes{0, {}, TextureHandle{}, desc.width, desc.height});
			return RenderTargetHandle{raw};
		}

		GLRenderTargetRes res;
		res.width = desc.width;
		res.height = desc.height;

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
		GpuHandle raw = m_resources->pool<GLViewRes>(ResourceType::View).create(GLViewRes{desc});
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
	}
	[[nodiscard]] RenderTargetHandle OpenGLRenderer::getBackBufferTarget() const {
		return m_backBufferTarget;
	}
	void OpenGLRenderer::setViewOrder(const std::vector<ViewHandle> &order) { m_viewOrder = order; }

	void OpenGLRenderer::beginFrame(float time) {
		m_currentTime = time;

		auto &viewPool = m_resources->pool<GLViewRes>(ResourceType::View);
		viewPool.forEachAlive([](GLViewRes &res) { res.queue.clear(); });
	}
	void OpenGLRenderer::submit(ViewHandle view, const DrawCommand &cmd) {
		auto *res = m_resources->pool<GLViewRes>(ResourceType::View).get(view.raw);
		if (!res) {
			setLastError("submit: invalid or stale view handle");
			return;
		}
		res->queue.push_back(cmd);
	}
	void OpenGLRenderer::endFrame() {
		auto &viewPool = m_resources->pool<GLViewRes>(ResourceType::View);

		for (ViewHandle handle : m_viewOrder) {
			GLViewRes *view = viewPool.get(handle.raw);
			if (!view) {
				setLastError("endFrame: setViewOrder contains invalid or stale view handle");
				continue;
			}
			renderView(*view);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLRenderer::present() { glfwSwapBuffers(m_window); }

	void OpenGLRenderer::renderView(GLViewRes &view) {
		std::sort(view.queue.begin(), view.queue.end(),
		          [](const DrawCommand &a, const DrawCommand &b) { return a.sortKey < b.sortKey; });

		auto &targetPool = m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget);
		auto *targetRes = targetPool.get(view.desc.target.raw);
		if (!targetRes) {
			setLastError("renderView: view has invalid render target");
			return;
		}

		bindRenderTarget(*targetRes, view.desc);

		RenderPass currentPass = static_cast<RenderPass>(255); // force first apply
		GpuHandle currentShader = kInvalidGpuHandle;
		GpuHandle currentGeometry = kInvalidGpuHandle;

		auto &shaderPool = m_resources->pool<GLShaderRes>(ResourceType::Shader);
		auto &geomPool = m_resources->pool<GLGeometryRes>(ResourceType::Geometry);

		for (const auto &cmd : view.queue) {
			if (cmd.pass != currentPass) {
				applyPassState(cmd.pass);
				currentPass = cmd.pass;
			}

			GLShaderRes *shaderRes = bindShaderIfNeeded(cmd.shader.raw, currentShader, view);
			if (!shaderRes) {
				setLastError("renderView: invalid shader handle in DrawCommand");
				continue;
			}

			if (shaderRes->modelLocation != -1)
				glUniformMatrix4fv(shaderRes->modelLocation, 1, GL_FALSE, &cmd.transform.data[0]);

			GLGeometryRes *geomRes = bindGeometryIfNeeded(cmd.geometry.raw, currentGeometry);
			if (!geomRes) {
				setLastError("renderView: invalid geometry handle in DrawCommand");
				continue;
			}

			applyMaterialUniforms(cmd);
			bindTextures(cmd.textures);

			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cmd.indexCount), GL_UNSIGNED_INT,
			               reinterpret_cast<void *>(
			                   static_cast<uintptr_t>(cmd.indexOffset * sizeof(uint32_t))));
		}
	}
	void OpenGLRenderer::bindRenderTarget(const GLRenderTargetRes &target, const ViewDesc &desc) {
		glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
		glViewport(desc.viewport.x, desc.viewport.y, desc.viewport.width, desc.viewport.height);

		GLbitfield clearMask = GL_COLOR_BUFFER_BIT;
		glClearColor(desc.clearColor.r, desc.clearColor.g, desc.clearColor.b, desc.clearColor.a);
		if (desc.clearDepth) {
			glDepthMask(GL_TRUE); // depth writes may be off from the previous pass (e.g. UI)
			clearMask |= GL_DEPTH_BUFFER_BIT;
		}
		glClear(clearMask);
	}
	void OpenGLRenderer::applyPassState(RenderPass pass) {
		switch (pass) {
			case RenderPass::Shadow:
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);
				break;

			case RenderPass::Opaque:
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);
				break;

			case RenderPass::Transparent:
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;

			case RenderPass::PostProcess:
				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glDisable(GL_BLEND);
				break;

			case RenderPass::UI:
				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;

			case RenderPass::Count:
				break;
		}
	}
	GLShaderRes *OpenGLRenderer::bindShaderIfNeeded(GpuHandle handle, GpuHandle &currentShader,
	                                                const GLViewRes &view) {
		auto &shaderPool = m_resources->pool<GLShaderRes>(ResourceType::Shader);
		GLShaderRes *shaderRes = shaderPool.get(handle);
		if (!shaderRes)
			return nullptr;

		if (handle != currentShader) {
			glUseProgram(shaderRes->program);
			currentShader = handle;

			if (shaderRes->viewLocation != -1)
				glUniformMatrix4fv(shaderRes->viewLocation, 1, GL_FALSE,
				                   &view.desc.camera.viewMatrix.data[0]);
			if (shaderRes->projectionLocation != -1)
				glUniformMatrix4fv(shaderRes->projectionLocation, 1, GL_FALSE,
				                   &view.desc.camera.projectionMatrix.data[0]);
		}
		return shaderRes;
	}
	GLGeometryRes *OpenGLRenderer::bindGeometryIfNeeded(GpuHandle handle,
	                                                    GpuHandle &currentGeometry) {
		auto &geomPool = m_resources->pool<GLGeometryRes>(ResourceType::Geometry);
		GLGeometryRes *geomRes = geomPool.get(handle);
		if (!geomRes)
			return nullptr;

		if (handle != currentGeometry) {
			glBindVertexArray(geomRes->vao);
			currentGeometry = handle;
		}
		return geomRes;
	}
	void OpenGLRenderer::applyMaterialUniforms(const DrawCommand &cmd) const {
		if (!cmd.uniformData || cmd.uniformSize == 0)
			return;

		glBindBuffer(GL_UNIFORM_BUFFER, m_materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, gfx::kMaxUniformBlockSize, nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, cmd.uniformSize, cmd.uniformData);
	}
	void OpenGLRenderer::bindTextures(const std::array<TextureHandle, kMaxTextureSlots> &textures) {
		auto &texPool = m_resources->pool<GLTextureRes>(ResourceType::Texture);
		for (uint32_t slot = 0; slot < kMaxTextureSlots; ++slot) {
			if (!textures[slot].isValid())
				continue;
			GLTextureRes *texRes = texPool.get(textures[slot].raw);
			if (!texRes)
				continue;
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(texRes->target, texRes->id);
		}
	}

	const char *OpenGLRenderer::getLastError() const { return m_lastError.c_str(); }
	void OpenGLRenderer::setLastError(std::string msg) const { m_lastError = std::move(msg); }
} // namespace triple::gl