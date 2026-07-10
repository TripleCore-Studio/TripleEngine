#pragma once

#include <unordered_map>

#include "triple/game/event/AssetEvent.h"
#include "triple/gfx/GfxTypes.h"

namespace triple::gfx {
	class IRenderer;
}

namespace triple::core {
	class EventBus;
}

namespace triple::game {
	class AssetManager;

	struct GpuResourceKey {
		AssetType type;
		AssetID id;

		bool operator==(const GpuResourceKey &other) const = default;
	};

	struct GpuResourceKeyHash {
		size_t operator()(const GpuResourceKey &key) const {
			size_t h1 = std::hash<uint8_t>{}(static_cast<uint8_t>(key.type));
			size_t h2 = std::hash<uint32_t>{}(key.id.slot);
			size_t h3 = std::hash<uint32_t>{}(key.id.generation);

			size_t seed = h1;
			seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};

	class GpuResourceRegistry {
	public:
		GpuResourceRegistry(gfx::IRenderer *renderer, core::EventBus *bus, AssetManager *manager);
		[[nodiscard]] gfx::GpuHandle resolve(GpuResourceKey key);

	private:
		void onAssetEvent(AssetEvent &event);

	private:
		gfx::IRenderer *m_renderer;
		core::EventBus *m_bus;
		AssetManager *m_assetManager;

	private:
		std::unordered_map<GpuResourceKey, gfx::GpuHandle, GpuResourceKeyHash> m_handles;
	};
} // namespace triple::game