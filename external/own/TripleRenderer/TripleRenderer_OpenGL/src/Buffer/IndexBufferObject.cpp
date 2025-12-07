#include "Buffer/IndexBufferObject.h"
#include <glad/glad.h>

namespace TripleEngineCore::TripleRenderer {
	constexpr GLenum usageToGLenum(IndexBufferObject::Usage usage) {
		switch (usage) {
		case IndexBufferObject::Usage::STATIC_DRAW:
			return GL_STATIC_DRAW;
		case IndexBufferObject::Usage::DYNAMIC_DRAW:
			return GL_DYNAMIC_DRAW;
		case IndexBufferObject::Usage::STREAM_DRAW:
			return GL_STREAM_DRAW;
		default:
			return GL_STATIC_DRAW;
		}
	}

	IndexBufferObject::IndexBufferObject()
	{
		glGenBuffers(1, &_iboId);
	}

	IndexBufferObject::~IndexBufferObject()
	{
		if (_iboId != 0)
			glDeleteBuffers(1, &_iboId);
	}

	void IndexBufferObject::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboId);
	}

	void IndexBufferObject::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void IndexBufferObject::setData(const void* data, unsigned int size, Usage usage)
	{
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usageToGLenum(usage));
		unbind();
	}
}
