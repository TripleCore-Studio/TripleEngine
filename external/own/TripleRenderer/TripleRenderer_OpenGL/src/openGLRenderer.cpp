#include "OpenGLRenderer.h"
#include <glad/glad.h>
#include <math.h>
#include <unordered_map>
#include "TLogger.h"

#include "Graphics/CameraData.h"

using namespace TripleEngineCore::TripleMath;

namespace TripleRenderer::GLRenderer {
    void OpenGLRenderer::Initialize() {
        if (!_initGlad) {
            TripleLogger::TLogger::ModuleCritical("OpenGLRenderer", "GLAD not initialized. Call initGlad() before Initialize().");
            return;
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        _pResourceManager = std::make_unique<Resources::RenderResourceManager>();
    }

    bool OpenGLRenderer::initGlad(void* loader) {
        bool result = gladLoadGLLoader((GLADloadproc)loader) != 0;
        _initGlad = result;
        return result;
    }

    void OpenGLRenderer::BeginFrame(float time)
    {
    }

    void OpenGLRenderer::RenderFrame(tecg::FrameContext& ctx)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tecg::CameraData camera = ctx.cameras[ctx.cameraIndex];

        Mat4 VP = camera.proj * camera.view;

        for (auto& cmd : ctx.commands) {
            Mat4 MVP = VP * cmd.worldMat;
            for (auto& item : cmd.items) {
                const Resources::GLGeometry* geom = _pResourceManager->getGLGeometry(item.geometry);
                Resources::GLShader* shader = _pResourceManager->getGLShader(item.material.shaderHandle);

                shader->bind();
                shader->setUniformMat4("u_MVP", MVP.data);
                shader->setUniform3fv("u_CameraPos", camera.pos.data());
                shader->setUniform1f("u_Time", ctx.time);
                
                geom->vao.bind();
                glDrawElements(GL_TRIANGLES, item.indexCount, GL_UNSIGNED_INT, (void*)(item.indexOffset * sizeof(uint32_t)));
                geom->vao.unbind();
            }
        }
    }


    void OpenGLRenderer::EndFrame()
    {
    }

    void OpenGLRenderer::SetViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    tec::GPUHandle OpenGLRenderer::UploadTexture(const tecg::TextureDesc& texture) {
        return _pResourceManager->createGLTexture(texture);
    }

    tec::GPUHandle OpenGLRenderer::UploadShader(const tecg::ShaderDesc& shader) {
        return _pResourceManager->createGLShader(shader);
    }

    tec::GPUHandle OpenGLRenderer::UploadGeometry(const tecg::GeometryDesc& geometry) {
        return _pResourceManager->createGLGeometry(geometry);
    }

    bool OpenGLRenderer::UnloadTexture(tec::GPUHandle handle) {
        return false;
    }

    bool OpenGLRenderer::UnloadShader(tec::GPUHandle handle) {
        return false;
    }

    bool OpenGLRenderer::UnloadGeometry(tec::GPUHandle handle) {
        return false;
    }

    void OpenGLRenderer::Shutdown() {}
}
RENDERER_API TripleEngineCore::IRenderer* CreateRenderer() {
    return new TripleRenderer::GLRenderer::OpenGLRenderer();
}

RENDERER_API void DestroyRenderer(TripleEngineCore::IRenderer* renderer) {
    delete renderer;
}