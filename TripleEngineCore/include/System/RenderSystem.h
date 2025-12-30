#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "Core/CoreTypes.h"

#include "Interfaces/ISystem.h"
#include "Interfaces/IRenderer.h"

#include "Scene/Scene.h"
#include "Graphics/RenderCommand.h"
#include "System/AssetsSystem.h"
#include "Asset/Asset.h"

#include <vector>
#include <unordered_map>

namespace TripleEngineCore::System {
	class RenderSystem : public ISystem {
	public:
		RenderSystem(AssetsSystem* assetsSystem) : _pAssets(assetsSystem) {}
		RenderSystem() = default;
		~RenderSystem() = default;

		virtual void update(float dt) {};
		virtual void init() {}
		virtual void shutdown() {}
		virtual SystemPriority priority() const { return SystemPriority::Late; }

		void uploadTexture(const Asset::Texture* texture);
		void uploadGeometry(const Asset::Model* model);
		void uploadShader(const Asset::Shader* shader);

		void buildRenderCommands(const Scene::Scene* scene, std::vector<Graphics::RenderCommand>& cmd);
		void setRenderer(IRenderer* renderer) { this->_pRenderer = renderer; }
		IRenderer* getRenderer() { return this->_pRenderer; }
	private:
		enum class ResourceType { Texture, Model, Shader };
		bool getGPU(ResourceType type, Asset::AssetID id, GPUHandle& out);

		void gatherFromObject(const Scene::SceneObject& obj,
			std::vector<Graphics::RenderCommand>& commands,
			const TripleMath::Mat4& parentWorld);

		void buildRenderCmd(Graphics::RenderCommand& cmd, const Asset::Model* obj);

		AssetsSystem* _pAssets = nullptr;
		IRenderer* _pRenderer = nullptr;

		std::unordered_map<Asset::AssetID, GPUHandle> _uploadedTextures;
		std::unordered_map<Asset::AssetID, GPUHandle> _uploadedModels;
		std::unordered_map<Asset::AssetID, GPUHandle> _uploadedShaders;
	};
}

#endif // RENDER_SYSTEM_H
