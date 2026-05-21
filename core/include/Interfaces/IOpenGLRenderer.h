#ifndef I_OPENGL_RENDERER_H
#define I_OPENGL_RENDERER_H

#include <ExportMacros.h>
#include "Interfaces/IRenderer.h"

namespace TripleEngineCore {
    class CORE_API IOpenGLRenderer : public virtual IRenderer {
    public:
        virtual bool initGlad(void* loader) = 0;
    };
}

#endif // I_OPENGL_RENDERER_H
