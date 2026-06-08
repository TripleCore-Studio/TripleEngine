#pragma once

#include "Event.h"

namespace triple::core {
	template <typename Derived>
	class EventBase : public Event {
	public:
		[[nodiscard]] EventID getTypeID() const override {
			return std::type_index(typeid(Derived));
		}

		static EventID staticTypeID() { return std::type_index(typeid(Derived)); }
	};
} // namespace triple::core