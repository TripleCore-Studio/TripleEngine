#ifndef VERTEX_BUFFER_OBJECT_H
#define VERTEX_BUFFER_OBJECT_H

namespace TripleRenderer::GLRenderer::Buffer {
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

		VertexBufferObject(VertexBufferObject&& other) noexcept;
		VertexBufferObject& operator=(VertexBufferObject&& other) noexcept;
	private:
		unsigned int m_ID;

		VertexBufferObject(const VertexBufferObject& program) = delete;
		VertexBufferObject& operator=(const VertexBufferObject& program) = delete;
	};
}

#endif // VERTEX_BUFFER_OBJECT_H
