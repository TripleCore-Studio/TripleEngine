#ifndef VERTEX_ARRAY_OBJECT_H
#define VERTEX_ARRAY_OBJECT_H

#include "VertexBufferObject.h"
#include "IndexBufferObject.h"

namespace triple::gl {
	class VertexArrayObject {
	  public:
		VertexArrayObject();
		~VertexArrayObject();
		void bind() const;
		void unbind() const;
		unsigned int getID() const { return m_vaoID; }
		void setData(const VertexBufferObject &vbo);
		void setIndexData(const IndexBufferObject &ibo);

		VertexArrayObject(const VertexArrayObject &) = delete;
		VertexArrayObject &operator=(const VertexArrayObject &) = delete;

		VertexArrayObject(VertexArrayObject &&other) noexcept;

		VertexArrayObject &operator=(VertexArrayObject &&other) noexcept;

	  private:
		unsigned int m_vaoID;
	};
} // namespace triple::gl

#endif // VERTEX_ARRAY_OBJECT_H
