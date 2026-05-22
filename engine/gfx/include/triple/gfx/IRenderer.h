#ifndef IRENDERER_H
#define IRENDERER_H

#include "GfxTypes.h"

#include "FrameContext.h"

#include "ShaderDesc.h"
#include "GeometryDesc.h"
#include "TextureDesc.h"

namespace triple::gfx {
    class IRenderer {
    public:
        virtual void Initialize() = 0;
        virtual GPUHandle UploadTexture(const TextureDesc& texture) = 0;
        virtual GPUHandle UploadShader(const ShaderDesc& shader) = 0;
        virtual GPUHandle UploadGeometry(const GeometryDesc& geometry) = 0;

        virtual bool UnloadTexture(GPUHandle handle) = 0;
        virtual bool UnloadShader(GPUHandle handle) = 0;
        virtual bool UnloadGeometry(GPUHandle handle) = 0;

		virtual void BeginFrame(float time) = 0;
        virtual void RenderFrame(FrameContext& ctx) = 0;
        virtual void EndFrame() = 0;
        virtual void Shutdown() = 0;
		virtual void SetViewport(int x, int y, int width, int height) = 0;
        virtual ~IRenderer() = default;
    };

    using CreateRendererFunc = IRenderer * (*)();
    using DestroyRendererFunc = void (*)(IRenderer*);
}
#endif // IRENDERER_H