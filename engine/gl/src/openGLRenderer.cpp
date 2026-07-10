#include "triple/gl/OpenGLRenderer.h"
#include <glad/glad.h>
#include <math.h>
#include "triple/log/Logger.h"

#include "triple/gfx/CameraData.h"

namespace triple::gl {
	void OpenGLRenderer::Initialize() {
		if (!m_initGlad) {
			triple::log::Logger::ModuleCritical(
			    "OpenGLRenderer", "GLAD not initialized. Call initGlad() before Initialize().");
			return;
		}

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		m_resourceManager = std::make_unique<RenderResourceManager>();
	}

	bool OpenGLRenderer::initGlad(void *loader) {
		bool result = gladLoadGLLoader((GLADloadproc)loader) != 0;
		m_initGlad = result;
		return result;
	}

	void OpenGLRenderer::BeginFrame(float time) {}

	void OpenGLRenderer::RenderFrame(gfx::FrameContext &ctx) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		triple::math::Mat4 VP = ctx.camera.proj * ctx.camera.view;

		GLShader *currentShader = nullptr;

		for (auto &cmd : ctx.commands) {
			triple::math::Mat4 MVP = VP * cmd.worldMat;

			for (auto &item : cmd.items) {
				const GLGeometry *geom = m_resourceManager->getGLGeometry(item.geometry);
				GLShader *shader = m_resourceManager->getGLShader(item.material.shaderHandle);
				GLTexture *albedo = m_resourceManager->getGLTexture(item.material.albedoTexHandle);

				if (shader != currentShader) {
					currentShader = shader;
					shader->bind();

					shader->setUniform3fv("u_CameraPos", ctx.camera.pos.data());
					shader->setUniform1f("u_Time", ctx.time);

					shader->setUniform3fv("u_Sun.direction", ctx.sunLight.direction.data());
					shader->setUniform3fv("u_Sun.color", ctx.sunLight.color.data());
					shader->setUniform1f("u_Sun.intensity", ctx.sunLight.intensity);

					shader->setUniform3fv("u_CamLight.color", ctx.cameraLight.color.data());
					shader->setUniform1f("u_CamLight.intensity", ctx.cameraLight.intensity);
					shader->setUniform1f("u_CamLight.radius", ctx.cameraLight.radius);

					shader->setUniform3fv("u_AmbientColor", ctx.ambientColor.data());
				}

				shader->setUniformMat4("u_MVP", MVP.data);
				shader->setUniformMat4("u_Model", cmd.worldMat.data);
				shader->setTexture("u_AlbedoTex", albedo->id, 0);

				geom->vao.bind();
				glDrawElements(GL_TRIANGLES, item.indexCount, GL_UNSIGNED_INT,
				               (void *)(item.indexOffset * sizeof(uint32_t)));
			}
		}

		glBindVertexArray(0);
	}

	void OpenGLRenderer::EndFrame() {}

	void OpenGLRenderer::SetViewport(int x, int y, int width, int height) {
		glViewport(x, y, width, height);
	}

	gfx::GPUHandle OpenGLRenderer::uploadTexture(const gfx::TextureDesc &texture) {
		return m_resourceManager->createGLTexture(texture);
	}

	gfx::GPUHandle OpenGLRenderer::uploadShader(const gfx::ShaderDesc &shader) {
		return m_resourceManager->createGLShader(shader);
	}

	gfx::GPUHandle OpenGLRenderer::uploadGeometry(const gfx::GeometryDesc &geometry) {
		return m_resourceManager->createGLGeometry(geometry);
	}

	bool OpenGLRenderer::UnloadTexture(gfx::GPUHandle handle) { return false; }

	bool OpenGLRenderer::UnloadShader(gfx::GPUHandle handle) { return false; }

	bool OpenGLRenderer::UnloadGeometry(gfx::GPUHandle handle) { return false; }

	void OpenGLRenderer::Shutdown() {}
} // namespace triple::gl
RENDERER_API triple::gfx::IRenderer *CreateRenderer() { return new triple::gl::OpenGLRenderer(); }

RENDERER_API void DestroyRenderer(triple::gfx::IRenderer *renderer) { delete renderer; }