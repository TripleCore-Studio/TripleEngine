#include "Engine/EventSystem.h"
#include <vector>

namespace TripleEngineCore {
	void EventDispatcher::addListener(Event::Type type, const EventCallbackFn& callback)
	{
		listeners[type].push_back(callback);
	}
	void EventDispatcher::dispatch(Event& event)
	{
		auto& cbs = listeners[event.getType()];
		for (auto& cb : cbs) {
			cb(event);
		}
	}
}

