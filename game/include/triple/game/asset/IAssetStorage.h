#pragma once

#include "AssetTypes.h"

namespace triple::game {
	class IAssetStorage {
	public:
		virtual ~IAssetStorage() = default;
		[[nodiscard]] virtual bool contains(AssetID id) const = 0;
		virtual bool remove(AssetID id) = 0;
	};
} // namespace triple::game