#ifndef IRENDERER_H
#define IRENDERER_H

#include <ExportMacros.h>

namespace TripleEngineCore {
    class CORE_API IRenderer {
    public:
        virtual void Initialize() = 0;
        virtual void RenderFrame() = 0;
        virtual void Shutdown() = 0;
        virtual ~IRenderer() = default;
    };

    using CreateRendererFunc = IRenderer * (*)();
    using DestroyRendererFunc = void (*)(IRenderer*);
}
#endif // IRENDERER_H