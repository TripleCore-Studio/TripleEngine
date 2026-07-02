#pragma once

#include <cstdint>

#include <triple/core/event/EventBase.h>

#include "triple/game/asset/AssetTypes.h"

namespace triple::game {
	enum class AssetEventKind : uint8_t {
		Loaded,
		Reloaded,
		Unloaded,
	};

	class AssetEvent : public core::EventBase<AssetEvent> {
	public:
		AssetEvent(AssetID id, AssetType type, AssetEventKind kind)
		    : m_id(id), m_type(type), m_kind(kind) {}

		[[nodiscard]] AssetID id() const { return m_id; }
		[[nodiscard]] AssetType type() const { return m_type; }
		[[nodiscard]] AssetEventKind kind() const { return m_kind; }

	private:
		AssetID m_id;
		AssetType m_type;
		AssetEventKind m_kind;
	};
} // namespace triple::game