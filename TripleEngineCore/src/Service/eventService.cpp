#include "Service/EventService.h"
#include <vector>

namespace TripleEngineCore::Service {
    void EventService::dispatch(Event::Event& event)
    {
        auto it = listeners.find(event.getTypeID());
        if (it == listeners.end())
            return;

        for (auto& fn : it->second)
            fn(event);
    }
}

