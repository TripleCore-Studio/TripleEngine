#pragma once

#include <cstdint>

#include <triple/core/base/CoreTypes.h>

namespace triple::game {
	using AssetID = core::VersionedIndex;
	inline constexpr AssetID kInvalidAssetId = core::kInvalidVersionedIndex;

	enum class AssetType : uint8_t {
		Texture,
		Shader,
		Material,
		Model,
		Count,
	};

	template <typename T>
	struct AssetTypeOf;

	template <typename T>
	struct TypedAssetID {
		AssetID raw = kInvalidAssetId;

		[[nodiscard]] constexpr bool isValid() const { return raw != kInvalidAssetId; }
		constexpr bool operator==(const TypedAssetID &) const = default;
	};
} // namespace triple::game