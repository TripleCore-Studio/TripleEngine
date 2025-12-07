#ifndef VERTEX_ARRAY_OBJECT_H
#define VERTEX_ARRAY_OBJECT_H

#include "Buffer/VertexBufferObject.h"
#include "Buffer/IndexBufferObject.h"

namespace TripleEngineCore {
	namespace TripleRenderer {
		class VertexArrayObject {
		public:
			VertexArrayObject();
			~VertexArrayObject();
			void bind() const;
			void unbind() const;
			unsigned int getID() const { return _vaoID; }
			void setData(const VertexBufferObject& vbo);
			void setIndexData(const IndexBufferObject& ibo);
		private:
			unsigned int _vaoID;
		};
	}
}
#endif // VERTEX_ARRAY_OBJECT_H
