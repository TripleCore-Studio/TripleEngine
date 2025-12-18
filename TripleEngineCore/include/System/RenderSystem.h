#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "Interfaces/ISystem.h"
#include "Scene/Scene.h"
#include "Graphics/RenderCommand.h"
#include "AssetsSystem.h"

#include <vector>

namespace TripleEngineCore::System {
	class RenderSystem : public ISystem {
	public:
		RenderSystem(AssetsSystem* assetsSystem) : assets(assetsSystem) {}
		RenderSystem() = default;
		~RenderSystem() = default;

		virtual void update(float dt) {};
		virtual void init() {}
		virtual void shutdown() {}
		virtual SystemPriority priority() const { return SystemPriority::Late; }

		void buildRenderCommands(const Scene::Scene& scene, std::vector<Graphics::RenderCommand>& cmd);

	private:
		void RenderSystem::gatherFromObject(const Scene::SceneObject& obj,
			std::vector<Graphics::RenderCommand>& commands,
			const TripleMath::Mat4& parentWorld);

		AssetsSystem* assets = nullptr;
	};
}

#endif // RENDER_SYSTEM_H
