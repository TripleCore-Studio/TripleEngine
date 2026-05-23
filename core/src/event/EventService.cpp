#include "triple/core/Event/EventService.h"

namespace triple::core {
    void EventService::dispatch(Event& event)
    {
        auto it = m_listeners.find(event.getTypeID());
        if (it == m_listeners.end())
            return;

        for (auto& fn : it->second)
            fn(event);
    }
}

