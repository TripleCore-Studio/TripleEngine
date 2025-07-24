#ifndef IRENDERER_H
#define IRENDERER_H

#include <ExportMacros.h>

namespace TripleEngineCore {
    class CORE_API IRenderer {
    public:
        virtual void Initialize() = 0;
        virtual void RenderFrame(float time) = 0;
        virtual void Shutdown() = 0;
		virtual void SetViewport(int x, int y, int width, int height) = 0;
        virtual ~IRenderer() = default;
    };

    using CreateRendererFunc = IRenderer * (*)();
    using DestroyRendererFunc = void (*)(IRenderer*);
}
#endif // IRENDERER_H