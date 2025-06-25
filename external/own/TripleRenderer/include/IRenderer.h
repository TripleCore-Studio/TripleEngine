#ifndef IRENDERER_H
#define IRENDERER_H

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
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;
        virtual void Init() = 0;
        virtual void Draw() = 0;
        virtual void Shutdown() = 0;
    };
}

#endif // IRENDERER_H