#ifndef IEVENT_SINK_H
#define IEVENT_SINK_H

#include "triple/core/event/Event.h"

namespace triple::core {
	class IEventSink {
	  public:
		virtual ~IEventSink() = default;
		virtual void pushEvent(Event &event) = 0;
	};
} // namespace triple::core

#endif // IEVENT_SINK_H
