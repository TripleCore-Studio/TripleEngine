#ifndef IRENDERER_H
#define IRENDERER_H

#include "Core/CoreTypes.h"

#include <ExportMacros.h>
#include "Graphics/FrameContext.h"

#include "Graphics/ShaderDesc.h"
#include "Graphics/GeometryDesc.h"
#include "Graphics/TextureDesc.h"

namespace TripleEngineCore {
    class CORE_API IRenderer {
    public:
        virtual void Initialize() = 0;
        virtual GPUHandle UploadTexture(const Graphics::TextureDesc& texture) = 0;
        virtual GPUHandle UploadShader(const Graphics::ShaderDesc& shader) = 0;
        virtual GPUHandle UploadGeometry(const Graphics::GeometryDesc& geometry) = 0;

        virtual bool UnloadTexture(GPUHandle handle) = 0;
        virtual bool UnloadShader(GPUHandle handle) = 0;
        virtual bool UnloadGeometry(GPUHandle handle) = 0;

		virtual void BeginFrame(float time) = 0;
        virtual void RenderFrame(Graphics::FrameContext& ctx) = 0;
        virtual void EndFrame() = 0;
        virtual void Shutdown() = 0;
		virtual void SetViewport(int x, int y, int width, int height) = 0;
        virtual ~IRenderer() = default;
    };

    using CreateRendererFunc = IRenderer * (*)();
    using DestroyRendererFunc = void (*)(IRenderer*);
}
#endif // IRENDERER_H