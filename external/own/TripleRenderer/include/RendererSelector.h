#ifndef RENDERER_SELECTOR_H
#define RENDERER_SELECTOR_H

#include "IRenderer.h"

#ifdef _WIN32
#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif
#else
#define RENDERER_API
#endif

namespace TripleRenderer {
    enum class RendererBackend {
        OpenGL,
        Vulkan
    };

    class RENDERER_API RendererSelector {
    public:
        static IRenderer* Create(RendererBackend backend);
    };
}


#endif // RENDERER_SELECTOR_H