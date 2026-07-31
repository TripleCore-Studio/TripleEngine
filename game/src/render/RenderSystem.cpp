#include "triple/game/render/RenderSystem.h"

#include <triple/game/asset/Model.h>
#include <triple/game/asset/Material.h>

#include <triple/math/Vec4.h>
#include <triple/math/MathCommon.h>
#include <triple/math/Mat4Operations.h>

#include <triple/log/Logger.h>

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/MeshRendererComponent.h"

#include "triple/game/render/GpuResourceRegistry.h"

#include "triple/game/asset/AssetManager.h"

#include "triple/game/utils/MaterialUtils.h"

namespace triple::game {
	[[nodiscard]] static gfx::RenderPass passFromBlendMode(MaterialBlendMode mode) {
		switch (mode) {
			case MaterialBlendMode::Opaque:
			case MaterialBlendMode::AlphaCutoff: // same GPU state as Opaque for now; shader may
			                                     // discard()
				return gfx::RenderPass::Opaque;
			case MaterialBlendMode::Transparent:
				return gfx::RenderPass::Transparent;
		}
		assert(false && "Unknown MaterialBlendMode");
		return gfx::RenderPass::Opaque;
	}

	[[nodiscard]] static uint32_t floatToSortableUint(float f) {
		uint32_t bits;
		std::memcpy(&bits, &f, sizeof(f));
		uint32_t mask = -int32_t(bits >> 31) | 0x80000000;
		return bits ^ mask;
	}

	[[nodiscard]] static uint64_t makeSortKey(gfx::RenderPass pass, gfx::ShaderHandle shader,
	                                          gfx::GeometryHandle geometry,
	                                          const math::Vec3 &worldPosition,
	                                          const math::Vec3 &cameraPosition) {
		if (pass == gfx::RenderPass::Transparent) {
			float distance = math::lengthSquared(worldPosition - cameraPosition);
			uint32_t depthBits = floatToSortableUint(distance);
			uint32_t invertedDepth = 0xFFFFFFFF - depthBits; // far objects first (back-to-front)
			return (uint64_t(pass) << 56) | (uint64_t(invertedDepth) << 24);
		}

		return (uint64_t(pass) << 56) | (uint64_t(shader.raw.slot) << 32) |
		       uint64_t(geometry.raw.slot);
	}

	void RenderSystem::submitScene(entt::registry &registry, gfx::ViewHandle view,
	                               gfx::FrameArena &arena, const math::Vec3 &cameraPosition) {
		auto sceneView = registry.view<TransformComponent, MeshRendererComponent>();

		for (auto [entity, transform, meshRenderer] : sceneView.each()) {
			const Model *model = s_assetManager->storageFor<Model>().get(meshRenderer.model);

			gfx::GeometryHandle geometry = gfx::GeometryHandle{
			    s_registry->resolve({AssetType::Model, meshRenderer.model.raw})};

			for (size_t meshIdx = 0; meshIdx < model->meshes.size(); ++meshIdx) {
				const Mesh &mesh = model->meshes[meshIdx];

				for (size_t primIdx = 0; primIdx < mesh.primitives.size(); ++primIdx) {
					const Primitive &prim = mesh.primitives[primIdx];
					const MaterialInstance &instance =
					    meshRenderer.materialInstances[meshIdx][primIdx];

					submitPrimitive(prim, instance, geometry, transform.worldMatrix, view, arena,
					                cameraPosition);
				}
			}
		}
	}

	void RenderSystem::submitPrimitive(const Primitive &prim, const MaterialInstance &instance,
	                                   gfx::GeometryHandle geometry, const math::Mat4 &worldMatrix,
	                                   gfx::ViewHandle view, gfx::FrameArena &arena,
	                                   const math::Vec3 &cameraPosition) {
		const Material *material = s_assetManager->storageFor<Material>().get(instance.base());
		const Shader *shader = s_assetManager->storageFor<Shader>().get(material->shader);

		gfx::DrawCommand cmd;
		cmd.geometry = geometry;
		cmd.indexOffset = prim.indexOffset;
		cmd.indexCount = prim.indexCount;
		cmd.shader =
		    gfx::ShaderHandle{s_registry->resolve({AssetType::Shader, material->shader.raw})};
		cmd.transform = worldMatrix;
		cmd.pass = passFromBlendMode(material->blendMode);

		MaterialUtils::packMaterialInstance(instance, *material, shader->desc, arena, *s_registry,
		                                    cmd);

		cmd.sortKey =
		    makeSortKey(cmd.pass, cmd.shader, cmd.geometry,
		                math::Mat4Operations::getTranslation(worldMatrix), cameraPosition);

		s_renderer->submit(view, cmd);
	}
} // namespace triple::game