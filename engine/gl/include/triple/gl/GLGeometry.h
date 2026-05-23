#ifndef MESH_GPU
#define MESH_GPU

#include <utility>

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "IndexBufferObject.h"

namespace triple::gl {
	struct GLGeometry {
		VertexArrayObject vao;
		VertexBufferObject vbo;
		IndexBufferObject ibo;

		GLGeometry() = default;

		GLGeometry(const GLGeometry &) = delete;
		GLGeometry &operator=(const GLGeometry &) = delete;

		GLGeometry(GLGeometry &&other) noexcept
		    : vao(std::move(other.vao)), vbo(std::move(other.vbo)), ibo(std::move(other.ibo)) {}

		GLGeometry &operator=(GLGeometry &&other) noexcept {
			if (this != &other) {
				vao = std::move(other.vao);
				vbo = std::move(other.vbo);
				ibo = std::move(other.ibo);
			}
			return *this;
		}
	};
} // namespace triple::gl

#endif // MESH_GPU
