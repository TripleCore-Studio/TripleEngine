#include <cstddef>
#include <glad/glad.h>
#include "Buffer/vertexArrayObject.h"
#include "Graphics/Vertex.h"

using namespace TripleEngineCore::Graphics;

namespace TripleEngineCore {
	namespace TripleRenderer {
		VertexArrayObject::VertexArrayObject()
		{
			glGenVertexArrays(1, &_vaoID);
		}
		VertexArrayObject::~VertexArrayObject()
		{
			if (_vaoID != 0)
				glDeleteVertexArrays(1, &_vaoID);
		}
		void VertexArrayObject::bind() const
		{
			glBindVertexArray(_vaoID);
		}
		void VertexArrayObject::unbind() const
		{
			glBindVertexArray(0);
		}
		void VertexArrayObject::setData(const VertexBufferObject& vbo)
		{
			this->bind();
			vbo.bind();

			// position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
			glEnableVertexAttribArray(0);

			// uv
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
			glEnableVertexAttribArray(1);

			this->unbind();
			vbo.unbind();
		}

		void VertexArrayObject::setIndexData(const IndexBufferObject& ibo)
		{
			this->bind();
			ibo.bind();
			this->unbind();
		}
	}
}