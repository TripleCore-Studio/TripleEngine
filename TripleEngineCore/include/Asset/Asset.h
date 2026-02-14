#ifndef ASSET_H
#define ASSET_H

#include <string>
#include "Core/CoreTypes.h"

namespace TripleEngineCore::Asset {
	using AssetID = TripleEngineCore::Index;
	inline constexpr AssetID INVALID_ASSET_ID = TripleEngineCore::INVALID_INDEX;

	struct Asset {
		std::string name;
		AssetID id;
		virtual ~Asset() = default;
	};
}

#endif // ASSET_H
