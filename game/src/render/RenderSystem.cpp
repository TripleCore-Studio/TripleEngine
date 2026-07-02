#include "triple/game/render/RenderSystem.h"

#include <triple/gfx/RenderItem.h>
#include <triple/game/asset/Model.h>
#include <triple/game/asset/Material.h>

#include <triple/math/Vec4.h>
#include <triple/log/Logger.h>

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/MeshComponent.h"

#include "triple/game/render/GpuResourceRegistry.h"

#include "triple/game/asset/DefaultAssets.h"
#include "triple/game/asset/AssetManager.h"

namespace triple::game {
	void RenderSystem::buildRenderCmd(gfx::RenderCommand &cmd, const Model *model,
	                                  gfx::GPUHandle geometryHandle) {
		if (!model)
			return;

		for (auto &mesh : model->meshes) {
			for (auto &p : mesh.primitives) {
				gfx::RenderItem item;

				const Material *mat = s_assetManager->storageFor<Material>().get(p.material);
				if (!mat) {
					TypedAssetID<Material> defaultId =
					    s_assetManager->storageFor<Material>().findByName(
					        std::string(kDefaultMaterialName));
					mat = s_assetManager->storageFor<Material>().get(defaultId);
				}
				if (!mat) {
					triple::log::Logger::ModuleWarn("RenderSystem", "Primitive in mesh({}) skipped",
					                                mesh.name);
					continue;
				}

				gfx::RenderMaterial rMat;
				rMat.albedoColor = mat->albedoColor;
				rMat.metallic = mat->metallic;
				rMat.roughness = mat->roughness;

				rMat.albedoTexHandle =
				    s_registry->resolve(GpuResourceKey{AssetType::Texture, mat->albedoTexture.raw});
				rMat.metallicTexHandle = s_registry->resolve(
				    GpuResourceKey{AssetType::Texture, mat->metallicTexture.raw});
				rMat.normalTexHandle =
				    s_registry->resolve(GpuResourceKey{AssetType::Texture, mat->normalTexture.raw});
				rMat.roughnessTexHandle = s_registry->resolve(
				    GpuResourceKey{AssetType::Texture, mat->roughnessTexture.raw});
				rMat.shaderHandle =
				    s_registry->resolve(GpuResourceKey{AssetType::Shader, mat->shader.raw});

				if (rMat.albedoTexHandle == gfx::kInvalidGpuHandle ||
				    rMat.metallicTexHandle == gfx::kInvalidGpuHandle ||
				    rMat.normalTexHandle == gfx::kInvalidGpuHandle ||
				    rMat.roughnessTexHandle == gfx::kInvalidGpuHandle ||
				    rMat.shaderHandle == gfx::kInvalidGpuHandle) {
					continue;
				}

				item.material = rMat;
				item.geometry = geometryHandle;
				item.indexCount = p.indexCount;
				item.indexOffset = p.indexOffset;

				cmd.items.push_back(item);
			}
		}
	}

	void RenderSystem::buildRenderCommands(entt::registry &reg,
	                                       std::vector<gfx::RenderCommand> &commands) {
		commands.clear();

		auto view = reg.view<TransformComponent, MeshComponent>();
		commands.reserve(view.size_hint());
		for (auto [entity, t, m] : view.each()) {
			if (!m.model.isValid() || s_assetManager == nullptr)
				continue;

			gfx::GPUHandle geometryHandle =
			    s_registry->resolve(GpuResourceKey{AssetType::Model, m.model.raw});
			if (geometryHandle == gfx::kInvalidGpuHandle)
				continue;

			const Model *model = s_assetManager->storageFor<Model>().get(m.model);

			gfx::RenderCommand cmd;
			cmd.worldMat = t.worldMatrix;

			buildRenderCmd(cmd, model, geometryHandle);

			if (!cmd.items.empty())
				commands.push_back(std::move(cmd));
		}
	}
} // namespace triple::game