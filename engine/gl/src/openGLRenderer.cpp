#include "triple/gl/OpenGLRenderer.h"
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <triple/log/Logger.h>

#include "triple/gl/resource/ResourceTypes.h"

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

		m_backBufferTarget = createBackBufferTarget(config.width, config.height);

		glGenBuffers(1, &m_materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, m_materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, gfx::kMaxUniformBlockSize, nullptr, GL_DYNAMIC_DRAW);
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
		    .forEachAlive([this](GLRenderTargetRes &res) {
			    if (res.type != gfx::RenderTargetType::BackBuffer) {
				    glDeleteFramebuffers(1, &res.fbo);
			    }
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

		prepareView(*targetRes, view);

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

	void OpenGLRenderer::prepareView(const GLRenderTargetRes &target, const GLViewRes &view) {
		const ViewDesc &desc = view.desc;

		glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
		glViewport(desc.viewport.x, desc.viewport.y, desc.viewport.width, desc.viewport.height);

		if (target.type == gfx::RenderTargetType::BackBuffer) {
			glDrawBuffer(GL_BACK);
		} else if (view.activeDrawBuffers.empty()) {
			glDrawBuffer(GL_NONE);
		} else {
			glDrawBuffers(static_cast<GLsizei>(view.activeDrawBuffers.size()),
			              view.activeDrawBuffers.data());
		}

		if (desc.clearDepth) {
			glDepthMask(GL_TRUE);
		}

		if (target.type == gfx::RenderTargetType::BackBuffer) {
			GLbitfield clearMask = GL_COLOR_BUFFER_BIT;

			glClearColor(desc.clearColor.r, desc.clearColor.g, desc.clearColor.b,
			             desc.clearColor.a);

			if (desc.clearDepth)
				clearMask |= GL_DEPTH_BUFFER_BIT;

			glClear(clearMask);
		} else {
			for (uint32_t attachmentIndex : desc.clearColorAttachments) {

				float cc[4] = {desc.clearColor.r, desc.clearColor.g, desc.clearColor.b,
				               desc.clearColor.a};

				glClearBufferfv(GL_COLOR, static_cast<GLint>(attachmentIndex), cc);
			}
			if (desc.clearDepth) {
				glClear(GL_DEPTH_BUFFER_BIT);
			}
		}
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

			case RenderPass::Lighting:
				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
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

	RenderTargetHandle OpenGLRenderer::createBackBufferTarget(uint32_t width, uint32_t height) {
		GLRenderTargetRes res{};
		res.fbo = 0;
		res.type = gfx::RenderTargetType::BackBuffer;
		res.width = width;
		res.height = height;

		GpuHandle raw =
		    m_resources->pool<GLRenderTargetRes>(ResourceType::RenderTarget).create(std::move(res));
		return RenderTargetHandle{raw};
	}

	const char *OpenGLRenderer::getLastError() const { return m_lastError.c_str(); }

	void OpenGLRenderer::setLastError(std::string msg) const { m_lastError = std::move(msg); }

	IRenderer *createRenderer() { return new OpenGLRenderer(); }

	void destroyRenderer(IRenderer *renderer) { delete renderer; }
} // namespace triple::gl