#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "Interfaces/IOpenGLRenderer.h"
#include "ExportMacros.h"
#include "Resources/RenderResourceManager.h"

#include <memory>

namespace tec = TripleEngineCore;
namespace tecg = TripleEngineCore::Graphics;

namespace TripleRenderer::GLRenderer
{
	class RENDERER_API OpenGLRenderer : public tec::IOpenGLRenderer {
	public:
		OpenGLRenderer() = default;
		virtual void Initialize() override;
		virtual void BeginFrame(float time) override;
		virtual void RenderFrame(tecg::FrameContext& ctx) override;
		virtual void EndFrame() override;
		virtual void Shutdown() override;
		virtual bool initGlad(void* loader) override;
		virtual void SetViewport(int x, int y, int width, int height) override;

		virtual tec::GPUHandle UploadTexture(const tecg::TextureDesc& texture) override;
		virtual tec::GPUHandle UploadShader(const tecg::ShaderDesc& shader) override;
		virtual tec::GPUHandle UploadGeometry(const tecg::GeometryDesc& geometry) override;

		virtual bool UnloadTexture(tec::GPUHandle handle) override;
		virtual bool UnloadShader(tec::GPUHandle handle) override;
		virtual bool UnloadGeometry(tec::GPUHandle handle) override;
	private:
		bool m_initGlad = false;
		std::unique_ptr<Resources::RenderResourceManager> m_resourceManager;
	};
}

#ifdef __cplusplus
extern "C" {
#endif

	RENDERER_API TripleEngineCore::IRenderer* CreateRenderer();
	RENDERER_API void DestroyRenderer(TripleEngineCore::IRenderer* renderer);

#ifdef __cplusplus
}
#endif

#endif // OPENGL_RENDERER_H