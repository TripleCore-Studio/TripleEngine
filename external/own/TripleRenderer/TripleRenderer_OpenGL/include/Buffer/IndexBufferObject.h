#ifndef INDEX_BUFFER_OBJECT_H
#define INDEX_BUFFER_OBJECT_H

namespace TripleRenderer::GLRenderer::Buffer {
    class IndexBufferObject {
    public:
        enum class Usage {
            STATIC_DRAW,
            DYNAMIC_DRAW,
            STREAM_DRAW
        };

        IndexBufferObject();
        ~IndexBufferObject();

        void bind() const;
        void unbind() const;

        void setData(const void* data, unsigned int size, Usage usage = Usage::STATIC_DRAW);

        unsigned int getID() const { return m_iboId; }

        IndexBufferObject(IndexBufferObject&& other) noexcept;
        IndexBufferObject& operator=(IndexBufferObject&& other) noexcept;
    private:
        unsigned int m_iboId;

        IndexBufferObject(const IndexBufferObject&) = delete;
        IndexBufferObject& operator=(const IndexBufferObject&) = delete;
    };
}

#endif // INDEX_BUFFER_OBJECT_H
