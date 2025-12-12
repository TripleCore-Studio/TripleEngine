#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "Interfaces/IOpenGLRenderer.h"
#include "ExportMacros.h"

namespace TripleEngineCore
{
	namespace TripleRenderer
	{
        class RENDERER_API OpenGLRenderer : public IOpenGLRenderer {
		public:
			OpenGLRenderer() = default;
			virtual void Initialize() override;
			virtual void BeginFrame(float time) override;
			virtual void RenderFrame(Graphics::FrameContext& ctx) override;
			virtual void EndFrame() override;
			virtual void Shutdown() override;
			virtual bool initGlad(void* loader) override;
			virtual void SetViewport(int x, int y, int width, int height) override;

			float gTime = 0.0f;
        };
	}
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