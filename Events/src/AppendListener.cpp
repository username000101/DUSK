#include "Events.hpp"

#include <algorithm>

bool events::EventsInteractions::append_listener(events::EventTypeId event_type, events::EventListenerSignature listener) {
    if (!listener)
        return false;

    bool is_overwriting = false;
    std::ranges::for_each(listeners, [&is_overwriting, &listener] (std::pair<std::uintptr_t, std::pair<EventTypeId, EventListenerSignature>> pair) {
        if (reinterpret_cast<std::uintptr_t>(listener) == pair.first)
            is_overwriting = true;
    });

    if (is_overwriting)
        logger->warn("Overwriting listener!");
    else
        logger->debug("Added a new listener with (addr){}",
                      reinterpret_cast<std::uintptr_t>(listener));
    listeners[reinterpret_cast<std::uintptr_t>(listener)] = std::make_pair(event_type, listener);
    return true;
}
