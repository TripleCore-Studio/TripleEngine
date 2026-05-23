#include "triple/gl/VertexBufferObject.h"
#include <glad/glad.h>

namespace triple::gl {
	constexpr GLenum usageToGLenum(VertexBufferObject::Usage usage) {
		switch (usage) {
		case VertexBufferObject::Usage::STATIC_DRAW:
			return GL_STATIC_DRAW;
		case VertexBufferObject::Usage::DYNAMIC_DRAW:
			return GL_DYNAMIC_DRAW;
		case VertexBufferObject::Usage::STREAM_DRAW:
			return GL_STREAM_DRAW;
		default:
			return GL_STATIC_DRAW;
		}
	}

	VertexBufferObject::VertexBufferObject() { glGenBuffers(1, &this->m_ID); }
	VertexBufferObject::~VertexBufferObject() {
		if (this->m_ID != 0) {
			glDeleteBuffers(1, &this->m_ID);
			this->m_ID = 0;
		}
	}
	void VertexBufferObject::bind() const { glBindBuffer(GL_ARRAY_BUFFER, this->m_ID); }
	void VertexBufferObject::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void VertexBufferObject::setData(const void *data, unsigned int size,
	                                 VertexBufferObject::Usage usage) {
		glBindBuffer(GL_ARRAY_BUFFER, this->m_ID);
		glBufferData(GL_ARRAY_BUFFER, size, data, usageToGLenum(usage));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	unsigned int VertexBufferObject::getID() const { return this->m_ID; }
	VertexBufferObject::VertexBufferObject(VertexBufferObject &&other) noexcept {
		this->m_ID = other.m_ID;
		other.m_ID = 0;
	}
	VertexBufferObject &VertexBufferObject::operator=(VertexBufferObject &&other) noexcept {
		if (this != &other) {
			if (this->m_ID != 0)
				glDeleteBuffers(1, &this->m_ID);
			this->m_ID = other.m_ID;
			other.m_ID = 0;
		}
		return *this;
	}
} // namespace triple::gl