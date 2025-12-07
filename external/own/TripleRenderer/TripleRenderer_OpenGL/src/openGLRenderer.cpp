#include "OpenGLRenderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>

#include "ShaderProgram.h"
#include "Struct/Vertex.h"
#include "Buffer/IndexBufferObject.h"
#include "Buffer/VertexArrayObject.h"
#include "Buffer/VertexBufferObject.h"

void TripleEngineCore::TripleRenderer::OpenGLRenderer::Initialize() {}

bool TripleEngineCore::TripleRenderer::OpenGLRenderer::initGlad(void* loader) {
    return gladLoadGLLoader((GLADloadproc)loader) != 0;
}

void TripleEngineCore::TripleRenderer::OpenGLRenderer::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

const char* vertexShaderSrc = R"(
#version 330 core

uniform mat4 u_MVP;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

out vec2 vUV;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
    vUV = aUV;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;

void main() {
    FragColor = vec4(vUV, 0.5, 1.0); // просто градиент по UV
}
)";

void TripleEngineCore::TripleRenderer::OpenGLRenderer::RenderFrame(float time) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	static bool isCompiled = false;
	static ShaderProgram shaderProgram;
	static VertexArrayObject vao;
	static VertexBufferObject vbo;
	static IndexBufferObject ibo;

    if (!isCompiled) {
		shaderProgram.compileProgram(vertexShaderSrc, fragmentShaderSrc);
        if (!shaderProgram.isCompiled()) {
            std::cout << "Shader program is not compiled!\n at: " << shaderProgram.getErrorLog() << std::endl;
			exit(EXIT_FAILURE);
        }
		isCompiled = true;
    }

    Vertex vertices[] = {
        {{-0.5f,  0.5f, 0.0f}, {0.5f, 1.0f}}, 
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},

        {{0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}},
		{{0.0f, -1.0f, 0.0f}, {0.5f, 1.0f}},

        {{-0.25f, 0.8f, 0.0f}, {0.5f, 1.0f}},
        {{0.25f, 0.8f, 0.0f}, {0.5f, 1.0f}},

        {{-0.25f, -0.8f, 0.0f}, {0.5f, 1.0f}},
        {{0.25f, -0.8f, 0.0f}, {0.5f, 1.0f}},

        {{-0.65f * sin(time * 1.5), cos(time), 0.0f}, {0.5f, 1.0f}},
        {{0.65f * sin(time * 1.5), cos(time), 0.0f}, {0.5f, 1.0f}},
    };

    uint32_t indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        1, 2, 5,
        0, 4, 6,
        3, 4, 7,
        1, 5, 8,
        2, 5, 9,
        0, 1, 10,
        3, 2, 11
    };

	vbo.setData(vertices, sizeof(vertices), VertexBufferObject::Usage::DYNAMIC_DRAW);
	ibo.setData(indices, sizeof(indices), IndexBufferObject::Usage::DYNAMIC_DRAW);

    vao.setData(vbo);
	vao.setIndexData(ibo);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(1, 2, 1));

    glm::mat4 view = glm::lookAt(
        glm::vec3(0, 0, 3),   // позиция камеры
        glm::vec3(0, 0, 0),   // куда смотрим
        glm::vec3(0, 1, 0)    // вверх
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(70.0f), // FOV
        1920.0f / 1080.0f,   // aspect ratio
        0.1f,                // near plane
        100.0f               // far plane
    );

    glm::mat4 mvp = projection * view * model;

	shaderProgram.setUniformMat4("u_MVP", glm::value_ptr(mvp));
	shaderProgram.use();
    vao.bind();
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
    vao.unbind();
}


void TripleEngineCore::TripleRenderer::OpenGLRenderer::Shutdown() {}

RENDERER_API TripleEngineCore::IRenderer* CreateRenderer() {
    return new TripleEngineCore::TripleRenderer::OpenGLRenderer();
}

RENDERER_API void DestroyRenderer(TripleEngineCore::IRenderer* renderer) {
    delete renderer;
}