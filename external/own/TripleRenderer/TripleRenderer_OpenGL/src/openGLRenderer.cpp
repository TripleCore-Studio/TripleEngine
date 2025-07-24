#include "OpenGLRenderer.h"
#include "ShaderProgram.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <cmath>
#include <chrono>
#include <algorithm>
namespace TripleEngineCore::TripleRenderer {

    void OpenGLRenderer::Initialize()
    {

    }

    bool OpenGLRenderer::initGlad(void* loader)
    {
        if (!gladLoadGLLoader((GLADloadproc)loader)) {
            return false;
        }
        return true;
    }

    void OpenGLRenderer::SetViewport(int x, int y, int width, int height)
    {
		glViewport(x, y, width, height);
    }

    void OpenGLRenderer::RenderFrame(float time) {

    }

	void OpenGLRenderer::Shutdown()
	{
		
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
