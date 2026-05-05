#include "OpenGLRenderer.h"
#include <glad/glad.h>
#include <math.h>
#include "TLogger.h"

#include "Graphics/CameraData.h"

using namespace TripleEngineCore::TripleMath;

namespace TripleRenderer::GLRenderer {
    void OpenGLRenderer::Initialize() {
        if (!m_initGlad) {
            TripleLogger::TLogger::ModuleCritical("OpenGLRenderer", "GLAD not initialized. Call initGlad() before Initialize().");
            return;
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        m_resourceManager = std::make_unique<Resources::RenderResourceManager>();
    }

    bool OpenGLRenderer::initGlad(void* loader) {
        bool result = gladLoadGLLoader((GLADloadproc)loader) != 0;
        m_initGlad = result;
        return result;
    }

    void OpenGLRenderer::BeginFrame(float time)
    {
    }

    void OpenGLRenderer::RenderFrame(tecg::FrameContext& ctx)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 VP = ctx.camera.proj * ctx.camera.view;

        for (auto& cmd : ctx.commands) {
            Mat4 MVP = VP * cmd.worldMat;
            for (auto& item : cmd.items) {
                const Resources::GLGeometry* geom = m_resourceManager->getGLGeometry(item.geometry);
                Resources::GLShader* shader = m_resourceManager->getGLShader(item.material.shaderHandle);
				Resources::GLTexture* albedo = m_resourceManager->getGLTexture(item.material.albedoTexHandle);

                shader->bind();
                shader->setUniformMat4("u_MVP", MVP.data);
                shader->setUniformMat4("u_Model", cmd.worldMat.data);
                shader->setUniform3fv("u_CameraPos", ctx.camera.pos.data());
                shader->setUniform1f("u_Time", ctx.time);
				shader->setTexture("u_AlbedoTex", albedo->id, 0);
                
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
        return m_resourceManager->createGLTexture(texture);
    }

    tec::GPUHandle OpenGLRenderer::UploadShader(const tecg::ShaderDesc& shader) {
        return m_resourceManager->createGLShader(shader);
    }

    tec::GPUHandle OpenGLRenderer::UploadGeometry(const tecg::GeometryDesc& geometry) {
        return m_resourceManager->createGLGeometry(geometry);
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