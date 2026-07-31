#include "triple/game/utils/MaterialUtils.h"

namespace triple::game {
	void packMaterialInstance(const MaterialInstance &instance, const Material &baseMaterial,
	                          const gfx::ShaderDesc &shaderDesc, gfx::FrameArena &arena,
	                          GpuResourceRegistry &gpuRegistry, gfx::DrawCommand &outCmd) {
		// --- uniforms ---
		uint8_t *blob = arena.allocate(shaderDesc.uniformBlockSize);

		for (const auto &param : shaderDesc.uniforms) {
			const float *src = nullptr;

			auto overrideIt = instance.paramOverrides().find(param.name);
			if (overrideIt != instance.paramOverrides().end()) {
				src = overrideIt->second.data.data();
			} else {
				auto baseIt = baseMaterial.params.find(param.name);
				if (baseIt != baseMaterial.params.end()) {
					src = baseIt->second.data.data();
				} else {
					static constexpr std::array<float, 16> kZero{};
					src = kZero.data();
				}
			}

			std::memcpy(blob + param.offset, src, sizeOfUniform(param.type));
		}

		outCmd.uniformData = blob;
		outCmd.uniformSize = shaderDesc.uniformBlockSize;

		// --- textures ---
		for (const auto &slot : shaderDesc.textureSlots) {
			TypedAssetID<Texture> textureId;

			auto overrideIt = instance.textureOverrides().find(slot.name);
			if (overrideIt != instance.textureOverrides().end()) {
				textureId = overrideIt->second;
			} else {
				auto baseIt = baseMaterial.textures.find(slot.name);
				if (baseIt == baseMaterial.textures.end()) {
					continue;
				}
				textureId = baseIt->second;
			}

			if (slot.slot >= gfx::kMaxTextureSlots) {
				continue;
			}

			outCmd.textures[slot.slot] =
			    gfx::TextureHandle{gpuRegistry.resolve({AssetType::Texture, textureId.raw})};
		}
	}
} // namespace triple::game