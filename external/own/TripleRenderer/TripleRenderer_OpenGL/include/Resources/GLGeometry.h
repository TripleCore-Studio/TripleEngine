#ifndef MESH_GPU
#define MESH_GPU

#include <utility>

#include "Buffer/VertexArrayObject.h"
#include "Buffer/VertexBufferObject.h"
#include "Buffer/IndexBufferObject.h"

namespace TripleRenderer::GLRenderer::Resources {
    struct GLGeometry {
        Buffer::VertexArrayObject vao;
        Buffer::VertexBufferObject vbo;
        Buffer::IndexBufferObject ibo;

        GLGeometry() = default;

        GLGeometry(const GLGeometry&) = delete;
        GLGeometry& operator=(const GLGeometry&) = delete;

        GLGeometry(GLGeometry&& other) noexcept
            : vao(std::move(other.vao)),
            vbo(std::move(other.vbo)),
            ibo(std::move(other.ibo))
        {}

        GLGeometry& operator=(GLGeometry&& other) noexcept {
            if (this != &other) {
                vao = std::move(other.vao);
                vbo = std::move(other.vbo);
                ibo = std::move(other.ibo);
            }
            return *this;
        }
    };
}

#endif // MESH_GPU
