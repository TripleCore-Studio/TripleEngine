#ifndef ASSET_H
#define ASSET_H

#include <string>
#include "triple/core/base/CoreTypes.h"

namespace triple::game {
	using AssetID = core::Index;
	inline constexpr AssetID INVALID_ASSET_ID = core::INVALID_INDEX;

	struct Asset {
		std::string name;
		AssetID id;
		virtual ~Asset() = default;
	};
} // namespace triple::game

#endif // ASSET_H
