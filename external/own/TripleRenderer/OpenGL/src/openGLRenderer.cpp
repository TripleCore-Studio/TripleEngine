#include <iostream>
#include "OpenGLRenderer.h"

namespace TripleRenderer {
    void OpenGLRenderer::Init() {
        std::cout << "[OpenGL] Init\n";
    }
    void OpenGLRenderer::Draw() {
        std::cout << "[OpenGL] Draw\n";
    }
    void OpenGLRenderer::Shutdown() {
        std::cout << "[OpenGL] Shutdown\n";
    }
}