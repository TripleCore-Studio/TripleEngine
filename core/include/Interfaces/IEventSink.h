#ifndef IEVENT_SINK_H
#define IEVENT_SINK_H

#include "Event/Event.h"

namespace TripleEngineCore {
	class IEventSink {
	public:
		virtual ~IEventSink() = default;
		virtual void pushEvent(Event::Event& event) = 0;
	};
}

#endif // IEVENT_SINK_H
