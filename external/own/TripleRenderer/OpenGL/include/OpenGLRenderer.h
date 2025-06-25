#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

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

namespace TripleRenderer
{
    class RENDERER_API OpenGLRenderer : public IRenderer {
    public:
        void Init() override;
        void Draw() override;
        void Shutdown() override;
    };
}


#endif // OPENGL_RENDERER_H