#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <cstdint>
#include <limits>

namespace triple::core {
	using Index = uint32_t;
	inline constexpr Index kInvalidIndex = (std::numeric_limits<Index>::max)();

	struct VersionedIndex {
		uint32_t slot;
		uint32_t generation;

		constexpr bool operator==(const VersionedIndex &) const = default;
	};

	inline constexpr VersionedIndex kInvalidVersionedIndex{std::numeric_limits<uint32_t>::max(),
	                                                       std::numeric_limits<uint32_t>::max()};

	using EntityId = uint32_t;
	inline constexpr EntityId kInvalidEntityId = (std::numeric_limits<EntityId>::max)();
} // namespace triple::core

#endif // CORE_TYPES_H
