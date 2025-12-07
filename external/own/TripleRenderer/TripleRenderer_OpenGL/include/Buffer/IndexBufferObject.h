#ifndef INDEX_BUFFER_OBJECT_H
#define INDEX_BUFFER_OBJECT_H

namespace TripleEngineCore {
    namespace TripleRenderer {
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

            unsigned int getID() const { return _iboId; }

        private:
            unsigned int _iboId;
        };
    }
}
#endif // INDEX_BUFFER_OBJECT_H
