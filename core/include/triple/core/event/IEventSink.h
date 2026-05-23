#ifndef IEVENT_SINK_H
#define IEVENT_SINK_H

#include "triple/core/Event/Event.h"

namespace triple::core {
	class IEventSink {
	public:
		virtual ~IEventSink() = default;
		virtual void pushEvent(Event& event) = 0;
	};
}

#endif // IEVENT_SINK_H
