#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "Core/CoreTypes.h"

#include "Interfaces/ISystem.h"
#include "Interfaces/IRenderer.h"

#include "Scene/Scene.h"
#include "Graphics/RenderCommand.h"
#include "Service/AssetService.h"
#include "Asset/Asset.h"

#include <vector>
#include <unordered_map>

namespace TripleEngineCore::System {
	class RenderSystem : public ISystem {
	public:
		RenderSystem(Service::AssetService* assetsSrv) : m_assets(assetsSrv) {}
		RenderSystem() = default;
		~RenderSystem() = default;

		virtual void update(float dt) {};
		virtual void init() {}
		virtual void shutdown() {}
		virtual SystemPriority priority() const { return SystemPriority::Late; }

		void uploadTexture(const Asset::Texture* texture);
		void uploadGeometry(const Asset::Model* model);
		void uploadShader(const Asset::Shader* shader);

		void buildRenderCommands(Scene::Scene* scene, std::vector<Graphics::RenderCommand>& cmd);
		void setRenderer(IRenderer* renderer) { this->m_renderer = renderer; }
		IRenderer* getRenderer() { return this->m_renderer; }
	private:
		enum class ResourceType { Texture, Model, Shader };
		bool getGPU(ResourceType type, Asset::AssetID id, GPUHandle& out);

		void gatherFromEntity(
			Scene::Scene* scene,
			Scene::Entity e,
			std::vector<Graphics::RenderCommand>& commands,
			const TripleMath::Mat4& parentWorld);

		void buildRenderCmd(Graphics::RenderCommand& cmd, const Asset::Model* obj);

		Service::AssetService* m_assets = nullptr;
		IRenderer* m_renderer = nullptr;

		std::unordered_map<Asset::AssetID, GPUHandle> m_uploadedTextures;
		std::unordered_map<Asset::AssetID, GPUHandle> m_uploadedModels;
		std::unordered_map<Asset::AssetID, GPUHandle> m_uploadedShaders;
	};
}

#endif // RENDER_SYSTEM_H
