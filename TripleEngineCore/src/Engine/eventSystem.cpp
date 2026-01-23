#include "Engine/EventSystem.h"
#include <vector>

namespace TripleEngineCore {
    void EventDispatcher::dispatch(Event::Event& event)
    {
        auto it = listeners.find(event.getTypeID());
        if (it == listeners.end())
            return;

        for (auto& fn : it->second)
            fn(event);
    }
}

