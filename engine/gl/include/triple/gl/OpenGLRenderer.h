#pragma once

#include <memory>
#include <triple/gfx/IRenderer.h>

#include "triple/gl/resource/ResourceManager.h"

class GLFWwindow;

#if defined(_WIN32)
	#ifdef RENDERER_EXPORTS
		#define RENDERER_API __declspec(dllexport)
	#else
		#define RENDERER_API __declspec(dllimport)
	#endif
#else
	#define RENDERER_API __attribute__((visibility("default")))
#endif

namespace triple::gl {
	using namespace gfx;

	class RENDERER_API OpenGLRenderer : public gfx::IRenderer {
	public:
		OpenGLRenderer();

		// lifecycle
		bool initialize(const RendererConfig &config) override;
		void shutdown() override;
		void resize(uint32_t width, uint32_t height) override;

		// resources
		[[nodiscard]] TextureHandle uploadTexture(const TextureDesc &desc) override;
		[[nodiscard]] ShaderHandle uploadShader(const ShaderDesc &desc) override;
		[[nodiscard]] GeometryHandle uploadGeometry(const GeometryDesc &desc) override;

		void unloadTexture(TextureHandle handle) override;
		void unloadShader(ShaderHandle handle) override;
		void unloadGeometry(GeometryHandle handle) override;

		// render targets
		[[nodiscard]] RenderTargetHandle createRenderTarget(const RenderTargetDesc &desc) override;
		void destroyRenderTarget(RenderTargetHandle handle) override;
		[[nodiscard]] TextureHandle getRenderTargetTexture(RenderTargetHandle handle,
		                                                   uint32_t colorIndex = 0) const override;
		[[nodiscard]] TextureHandle
		getRenderTargetDepthTexture(RenderTargetHandle handle) const override;

		// views
		[[nodiscard]] ViewHandle createView(const ViewDesc &desc) override;
		void destroyView(ViewHandle handle) override;
		void updateView(ViewHandle handle, const ViewDesc &desc) override;
		[[nodiscard]] RenderTargetHandle getBackBufferTarget() const override;
		void setViewOrder(const std::vector<ViewHandle> &order) override;

		// frame
		void beginFrame(float time) override;
		void submit(ViewHandle view, const DrawCommand &cmd) override;
		void endFrame() override;
		void present() override;

		// debug
		[[nodiscard]] const char *getLastError() const override;

	private:
		void renderView(GLViewRes &view);
		void bindRenderTarget(const GLRenderTargetRes &target, const ViewDesc &desc);
		void applyPassState(RenderPass pass);
		GLShaderRes *bindShaderIfNeeded(GpuHandle handle, GpuHandle &currentShader,
		                                const GLViewRes &view);
		GLGeometryRes *bindGeometryIfNeeded(GpuHandle handle, GpuHandle &currentGeometry);
		void bindTextures(const std::array<TextureHandle, kMaxTextureSlots> &textures);
		void applyMaterialUniforms(const DrawCommand &cmd) const;

	private:
		void setLastError(std::string msg) const;
		GLuint compileStage(GLenum stage, const char *src);

	private:
		GLuint m_materialUbo = 0;
		static constexpr GLuint kMaterialUboBinding = 0;

		mutable std::string m_lastError;
		std::unique_ptr<ResourceManager> m_resources;
		std::vector<ViewHandle> m_viewOrder;
		RenderTargetHandle m_backBufferTarget;
		GLFWwindow *m_window = nullptr;
		float m_currentTime = 0;
	};

	extern "C" {

	RENDERER_API IRenderer *createRenderer() { return new OpenGLRenderer(); }

	RENDERER_API void destroyRenderer(IRenderer *renderer) { delete renderer; }

	} // extern "C"
} // namespace triple::gl