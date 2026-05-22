#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <functional>
#include <unordered_map>
#include <triple/core/ExportMacros.h>

#include "triple/core/Event/Event.h"
#include "triple/core/Interfaces/IEventSink.h"
#include "triple/core/Interfaces/IService.h"

namespace triple::core {
    class CORE_API EventService : public IEventSink, public IService {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        template<typename EventT>
        void addListener(std::function<void(EventT&)> callback) {

            auto wrapper = [callback](Event& e) {
                callback(static_cast<EventT&>(e));
            };

            m_listeners[typeid(EventT)].push_back(wrapper);
        }

        void dispatch(Event& event);

        void pushEvent(Event& event) override {
            dispatch(event);
        }
    private:
        std::unordered_map<EventID, std::vector<EventCallbackFn>> m_listeners;
    };
}
#endif // EVENT_SERVICE_H