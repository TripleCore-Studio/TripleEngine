#include "Service/EventService.h"
#include <vector>

namespace TripleEngineCore::Service {
    void EventService::dispatch(Event::Event& event)
    {
        auto it = m_listeners.find(event.getTypeID());
        if (it == m_listeners.end())
            return;

        for (auto& fn : it->second)
            fn(event);
    }
}

