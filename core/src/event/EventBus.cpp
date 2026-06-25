#include "triple/core/Event/EventBus.h"

namespace triple::core {
	void EventBus::dispatch(Event &event) {
		for (auto &fn : m_catchAllListeners)
			fn(event);

		auto it = m_listeners.find(event.getTypeID());
		if (it == m_listeners.end())
			return;

		for (auto &fn : it->second)
			fn(event);
	}
} // namespace triple::core
