#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "triple/gfx/IOpenGLRenderer.h"
#include "RenderResourceManager.h"

#include <memory>

// ===================== RENDERER ======================
#if defined(_WIN32) || defined(_WIN64)
#if defined(RENDERER_EXPORTS)
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif
#else
#define RENDERER_API
#endif

namespace triple::gl {
	class OpenGLRenderer : public gfx::IOpenGLRenderer {
	public:
		OpenGLRenderer() = default;
		virtual void Initialize() override;
		virtual void BeginFrame(float time) override;
		virtual void RenderFrame(gfx::FrameContext &ctx) override;
		virtual void EndFrame() override;
		virtual void Shutdown() override;
		virtual bool initGlad(void *loader) override;
		virtual void SetViewport(int x, int y, int width, int height) override;

		virtual gfx::GPUHandle uploadTexture(const gfx::TextureDesc &texture) override;
		virtual gfx::GPUHandle uploadShader(const gfx::ShaderDesc &shader) override;
		virtual gfx::GPUHandle uploadGeometry(const gfx::GeometryDesc &geometry) override;

		virtual bool UnloadTexture(gfx::GPUHandle handle) override;
		virtual bool UnloadShader(gfx::GPUHandle handle) override;
		virtual bool UnloadGeometry(gfx::GPUHandle handle) override;

	private:
		bool m_initGlad = false;
		std::unique_ptr<RenderResourceManager> m_resourceManager;
	};
} // namespace triple::gl

#ifdef __cplusplus
extern "C" {
#endif

RENDERER_API triple::gfx::IRenderer *CreateRenderer();
RENDERER_API void DestroyRenderer(triple::gfx::IRenderer *renderer);

#ifdef __cplusplus
}
#endif

#endif // OPENGL_RENDERER_H