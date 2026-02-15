#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <functional>
#include <unordered_map>
#include <ExportMacros.h>

#include "Event/Event.h"
#include "Interfaces/IEventSink.h"
#include "Interfaces/IService.h"

namespace TripleEngineCore::Service {
    class CORE_API EventService : public IEventSink, public IService {
    public:
        using EventCallbackFn = std::function<void(Event::Event&)>;

        template<typename EventT>
        void addListener(std::function<void(EventT&)> callback) {

            auto wrapper = [callback](Event::Event& e) {
                callback(static_cast<EventT&>(e));
            };

            listeners[typeid(EventT)].push_back(wrapper);
        }

        void dispatch(Event::Event& event);

        void pushEvent(Event::Event& event) override {
            dispatch(event);
        }
    private:
        std::unordered_map<Event::EventID, std::vector<EventCallbackFn>> listeners;
    };
}
#endif // EVENT_SERVICE_H