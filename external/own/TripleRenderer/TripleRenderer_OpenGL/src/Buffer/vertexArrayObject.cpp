#include <cstddef>
#include <glad/glad.h>
#include "Buffer/vertexArrayObject.h"
#include "Graphics/Vertex.h"

using namespace TripleEngineCore::Graphics;

namespace TripleRenderer::GLRenderer::Buffer {
	VertexArrayObject::VertexArrayObject()
	{
		glGenVertexArrays(1, &m_vaoID);
	}
	VertexArrayObject::~VertexArrayObject()
	{
		if (m_vaoID != 0)
			glDeleteVertexArrays(1, &m_vaoID);
	}

	VertexArrayObject::VertexArrayObject(VertexArrayObject&& other) noexcept
		: m_vaoID(other.m_vaoID)
	{
		other.m_vaoID = 0;
	}

	VertexArrayObject& VertexArrayObject::operator=(VertexArrayObject&& other) noexcept {
		if (this != &other) {
			if (m_vaoID != 0) glDeleteVertexArrays(1, &m_vaoID);
			m_vaoID = other.m_vaoID;
			other.m_vaoID = 0;
		}
		return *this;
	}

	void VertexArrayObject::bind() const
	{
		glBindVertexArray(m_vaoID);
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

		// normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		// tangent
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(2);

		// bitangent
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
		glEnableVertexAttribArray(3);

		// color
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(4);

		// uv
		glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(5);

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