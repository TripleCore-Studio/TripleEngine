#ifndef VERTEX_BUFFER_OBJECT_H
#define VERTEX_BUFFER_OBJECT_H

namespace TripleEngineCore {
	namespace TripleRenderer {
		class VertexBufferObject {
		public:
			enum class Usage {
				STATIC_DRAW,
				DYNAMIC_DRAW,
				STREAM_DRAW
			};

			VertexBufferObject();
			~VertexBufferObject();
			void bind() const;
			void unbind() const;
			void setData(const void* data, unsigned int size, Usage usage = Usage::STATIC_DRAW);
			unsigned int getID() const;
		private:
			unsigned int m_ID;

			VertexBufferObject(const VertexBufferObject& program) = delete;
			VertexBufferObject& operator=(const VertexBufferObject& program) = delete;

			VertexBufferObject(VertexBufferObject&& other) noexcept;
			VertexBufferObject& operator=(VertexBufferObject&& other) noexcept;
		};
	}
}

#endif // VERTEX_BUFFER_OBJECT_H
