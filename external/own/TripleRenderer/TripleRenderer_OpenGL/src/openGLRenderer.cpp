#include "OpenGLRenderer.h"
#include <glad/glad.h>
#include <iostream>
#include <math.h>
#include <unordered_map>

#include "ShaderProgram.h"
#include "Buffer/IndexBufferObject.h"
#include "Buffer/VertexArrayObject.h"
#include "Buffer/VertexBufferObject.h"

using namespace TripleEngineCore::TripleMath;

void TripleEngineCore::TripleRenderer::OpenGLRenderer::Initialize() {}

bool TripleEngineCore::TripleRenderer::OpenGLRenderer::initGlad(void* loader) {
    return gladLoadGLLoader((GLADloadproc)loader) != 0;
}

void TripleEngineCore::TripleRenderer::OpenGLRenderer::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void TripleEngineCore::TripleRenderer::OpenGLRenderer::BeginFrame(float time)
{
	this->gTime = time;
}

const char* vertexShaderSrc = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 u_MVP;
uniform float u_Time;

out vec3 vColor;
out vec3 vPosition;

void main()
{
    gl_Position = u_MVP * vec4(aPos, 1.0);
    vColor = aColor;
    vPosition = aPos;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core

in vec3 vColor;
in vec3 vPosition;
uniform float u_Time;
out vec4 FragColor;

void main()
{
    float heightFactor = clamp(vPosition.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 color = vColor * heightFactor;

    FragColor = vec4(color, 1.0);
}
)";

void TripleEngineCore::TripleRenderer::OpenGLRenderer::RenderFrame(Graphics::FrameContext& ctx)
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    using namespace TripleEngineCore::TripleRenderer;

    static ShaderProgram shader;
    static bool shaderInitialized = false;
    if (!shaderInitialized) {
        shader.compileProgram(vertexShaderSrc, fragmentShaderSrc);
        shaderInitialized = true;
    }

    struct MeshGPU {
        VertexArrayObject vao;
        VertexBufferObject vbo;
        IndexBufferObject ibo;
    };
    static std::unordered_map<const Graphics::Mesh*, std::unique_ptr<MeshGPU>> meshCache;

    for (auto& cam : ctx.cameras)
    {
        Mat4 VP = cam.proj * cam.view;

        for (auto& cmd : ctx.commands)
        {
            Mat4 MVP = VP * cmd.worldMat;

            for (auto* mesh : cmd.meshes)
            {
                if (!mesh) continue;

                if (meshCache.find(mesh) == meshCache.end())
                {
                    auto gpuMesh = std::make_unique<MeshGPU>();
                    gpuMesh->vbo.setData(mesh->vertices.data(), mesh->vertices.size() * sizeof(TripleEngineCore::Graphics::Vertex));
                    gpuMesh->ibo.setData(mesh->indices.data(), mesh->indices.size() * sizeof(uint32_t));
                    gpuMesh->vao.setData(gpuMesh->vbo);
                    gpuMesh->vao.setIndexData(gpuMesh->ibo);

                    meshCache[mesh] = std::move(gpuMesh);
                }

                MeshGPU* gpu = meshCache[mesh].get();
                shader.use();
                shader.setUniformMat4("u_MVP", MVP.data);
                shader.setUniform1f("u_Time", this->gTime);

                gpu->vao.bind();
                gpu->ibo.bind();
                glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
                gpu->vao.unbind();
                gpu->ibo.unbind();
            }
        }
    }
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