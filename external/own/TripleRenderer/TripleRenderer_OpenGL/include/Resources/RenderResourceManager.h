#ifndef RENDER_RESOURCE_MANAGER
#define RENDER_RESOURCE_MANAGER

#include <unordered_map>
#include <memory>

#include "Resources/MeshGPU.h"
#include "Resources/TextureGPU.h"
#include "ShaderProgram.h"
#include "RuntimeHash.h"

#include "Runtime/RuntimeMesh.h"
#include "Runtime/RuntimeTexture.h"
#include "Runtime/RuntimeShader.h"

namespace TripleEngineCore::TripleRenderer::Resources {
	class RenderResourceManager {
	public:
		MeshGPU* getMeshGPU(const Runtime::RuntimeMesh& mesh);
		TextureGPU* getTextureGPU(const Runtime::RuntimeTexture& texture);
		TextureID getTextureID(const Runtime::RuntimeTexture& texture);
		ShaderProgram* getShaderProgram(const Runtime::RuntimeShader& shader);

		void bindMaterial(const Runtime::RuntimeMaterial& material);
	private:
		std::unordered_map<RuntimeMeshKey, std::unique_ptr<MeshGPU>, RuntimeMeshHasher> meshCache;
		std::unordered_map<RuntimeTextureKey, std::unique_ptr<TextureGPU>, RuntimeTextureHasher> textureCache;
		std::unordered_map<RuntimeShaderKey, std::unique_ptr<ShaderProgram>, RuntimeShaderHasher> shaderCache;
	};
}

#endif // RENDER_RESOURCE_MANAGER
