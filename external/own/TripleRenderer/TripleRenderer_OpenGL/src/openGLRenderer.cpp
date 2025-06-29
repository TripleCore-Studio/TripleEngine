#include "OpenGLRenderer.h"
#include <glad/glad.h>

namespace TripleEngineCore::TripleRenderer {
    void OpenGLRenderer::Initialize()
    {

    }

    void OpenGLRenderer::RenderFrame()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

	void OpenGLRenderer::Shutdown()
	{
		
	}
    void OpenGLRenderer::initGlad(void* loader)
    {
        if (!gladLoadGLLoader((GLADloadproc)loader)) {
        }
        glClearColor(0, 0, 1, 1);
    }
}

RENDERER_API TripleEngineCore::IRenderer* CreateRenderer()
{
    return new TripleEngineCore::TripleRenderer::OpenGLRenderer();
}

RENDERER_API void DestroyRenderer(TripleEngineCore::IRenderer* renderer)
{
    delete renderer;
}
