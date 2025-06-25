#include "RendererSelector.h"

#if defined(USE_OPENGL)
#include "OpenGLRenderer.h"
#elif defined(USE_VULKAN)
#include "VulkanRenderer.h"
#endif

namespace TripleRenderer {
    IRenderer* RendererSelector::Create(RendererBackend backend) {
    #if defined(USE_OPENGL)
        return new OpenGLRenderer();
    #elif defined(USE_VULKAN)
        return new VulkanRenderer();
    #else
        return nullptr;
    #endif
    }
}