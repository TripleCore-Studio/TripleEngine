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
			OpenGLRenderer() {}
			virtual void Initialize() override;
			virtual void RenderFrame() override;
			virtual void Shutdown() override;
			virtual void initGlad(void* loader) override;
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