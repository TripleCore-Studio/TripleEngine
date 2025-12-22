#include "OpenGLRenderer.h"
#include <glad/glad.h>
#include <math.h>
#include <unordered_map>
#include "TLogger.h"

using namespace TripleEngineCore::TripleMath;

void TripleEngineCore::TripleRenderer::OpenGLRenderer::Initialize() {
    if(!_initGlad) {
        TripleLogger::TLogger::ModuleCritical("OpenGLRenderer", "GLAD not initialized. Call initGlad() before Initialize().");
        return;
	}

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    _pResourceManager = std::make_unique<Resources::RenderResourceManager>();
}

bool TripleEngineCore::TripleRenderer::OpenGLRenderer::initGlad(void* loader) {
	bool result = gladLoadGLLoader((GLADloadproc)loader) != 0;
	_initGlad = result;
    return result;
}

void TripleEngineCore::TripleRenderer::OpenGLRenderer::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void TripleEngineCore::TripleRenderer::OpenGLRenderer::BeginFrame(float time)
{
}

void TripleEngineCore::TripleRenderer::OpenGLRenderer::RenderFrame(Graphics::FrameContext& ctx)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (ctx.cameras.empty()) return;
    auto cam = ctx.cameras[ctx.cameraIndex];
    Mat4 VP = cam.proj * cam.view;

    ShaderProgram* currentShader = nullptr;
    VertexArrayObject* currentVAO = nullptr;

    for (const auto& cmd : ctx.commands) {
        if (cmd.items.empty())
            continue;

        Mat4 MVP = VP * cmd.worldMat;

        for (size_t i = 0; i < cmd.items.size(); i++) {
			const Runtime::RuntimeMesh& mesh = cmd.items[i].mesh;
			const Runtime::RuntimeMaterial& mat = mesh.material;

            Resources::MeshGPU* meshGpu = _pResourceManager->getMeshGPU(mesh);
            ShaderProgram* program = _pResourceManager->getShaderProgram(mat.shader);
			program->setUniformMat4("u_MVP", MVP.data);
			_pResourceManager->bindMaterial(mat);

            if (program != currentShader) {
                program->use();
                currentShader = program;
            }

            if (&meshGpu->vao != currentVAO) {
                meshGpu->vao.bind();
                currentVAO = &meshGpu->vao;
            }

            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
        }
    }

    if (currentVAO) currentVAO->unbind();
}


void TripleEngineCore::TripleRenderer::OpenGLRenderer::EndFrame()
{
}

void TripleEngineCore::TripleRenderer::OpenGLRenderer::Shutdown() {}

RENDERER_API TripleEngineCore::IRenderer* CreateRenderer() {
    return new TripleEngineCore::TripleRenderer::OpenGLRenderer();
}

RENDERER_API void DestroyRenderer(TripleEngineCore::IRenderer* renderer) {
    delete renderer;
}