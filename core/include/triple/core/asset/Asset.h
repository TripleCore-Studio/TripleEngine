#ifndef ASSET_H
#define ASSET_H

#include <string>
#include "triple/core/base/CoreTypes.h"

namespace triple::core {
	using AssetID = Index;
	inline constexpr AssetID INVALID_ASSET_ID = INVALID_INDEX;

	struct Asset {
		std::string name;
		AssetID id;
		virtual ~Asset() = default;
	};
}

#endif // ASSET_H
