#ifndef RENDER_RESOURCE_MANAGER
#define RENDER_RESOURCE_MANAGER

#include "Core/CoreTypes.h"

#include <unordered_map>
#include <memory>

#include "Resources/GLGeometry.h"
#include "Resources/GLTexture.h"
#include "Resources/GLShader.h"
#include "Graphics/RenderMaterial.h"

#include "Graphics/GeometryDesc.h"
#include "Graphics/TextureDesc.h"
#include "Graphics/ShaderDesc.h"

namespace tec = TripleEngineCore;

namespace TripleRenderer::GLRenderer::Resources {
	class RenderResourceManager {
	public:
		GLGeometry* getGLGeometry(tec::GPUHandle geometry);
		tec::GPUHandle createGLGeometry(const tec::Graphics::GeometryDesc& desc);
		GLTexture* getGLTexture(tec::GPUHandle texture);
		tec::GPUHandle createGLTexture(const tec::Graphics::TextureDesc& desc);
		GLShader* getGLShader(tec::GPUHandle shader);
		tec::GPUHandle createGLShader(const tec::Graphics::ShaderDesc& desc);

		void bindMaterial(tec::Graphics::RenderMaterial& material);
	private:
		std::unordered_map<tec::GPUHandle, GLGeometry> m_primitiveCache;
		std::unordered_map<tec::GPUHandle, GLTexture> m_textureCache;
		std::unordered_map<tec::GPUHandle, GLShader> m_shaderCache;
		tec::GPUHandle m_nextHandle = 0;
	};
}

#endif // RENDER_RESOURCE_MANAGER
