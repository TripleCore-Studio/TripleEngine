#ifndef ENGINE_LOADED_EVENT_H
#define ENGINE_LOADED_EVENT_H

#include "Event.h"

namespace triple::core {
	class CORE_API EngineLoadedEvent : public Event {
	public:
		EngineLoadedEvent() {}
		EventID getTypeID() const override {
			return typeid(EngineLoadedEvent);
		}
	};
}

#endif // ENGINE_LOADED_EVENT_H
