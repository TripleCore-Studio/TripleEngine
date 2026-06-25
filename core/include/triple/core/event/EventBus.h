#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <functional>
#include <unordered_map>

#include "triple/core/base/ExportMacros.h"
#include "IEventSink.h"
#include "Event.h"

namespace triple::core {
	class CORE_API EventBus : public IEventSink {
	public:
		using EventCallbackFn = std::function<void(Event &)>;

		template <typename EventT>
		void addListener(std::function<void(EventT &)> callback) {
			auto wrapper = [callback](Event &e) { callback(static_cast<EventT &>(e)); };
			m_listeners[typeid(EventT)].push_back(wrapper);
		}

		void addListener(EventCallbackFn callback) {
			m_catchAllListeners.push_back(std::move(callback));
		}

		void dispatch(Event &event);

		void pushEvent(Event &event) override { dispatch(event); }

	private:
		std::unordered_map<EventID, std::vector<EventCallbackFn>> m_listeners;
		std::vector<EventCallbackFn> m_catchAllListeners;
	};
} // namespace triple::core
#endif // EVENT_SERVICE_H