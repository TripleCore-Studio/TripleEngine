#ifndef MESH_GPU
#define MESH_GPU

#include "Buffer/VertexArrayObject.h"
#include "Buffer/VertexBufferObject.h"
#include "Buffer/IndexBufferObject.h"

namespace TripleEngineCore::TripleRenderer::Resources {
    struct MeshGPU {
        VertexArrayObject vao;
        VertexBufferObject vbo;
        IndexBufferObject ibo;
    };
}

#endif // MESH_GPU
