#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "Interfaces/IOpenGLRenderer.h"
#include "ExportMacros.h"
#include "Resources/RenderResourceManager.h"

#include <memory>

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
		private:
			bool _initGlad = false;
			std::unique_ptr<Resources::RenderResourceManager> _pResourceManager;
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