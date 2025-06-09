#include "Events.hpp"

bool events::EventsInteractions::remove_listener(events::EventListenerSignature listener) {
    if (!listener)
        return false;
    if (auto value = reinterpret_cast<std::uintptr_t>(listener); listeners.contains(value)) {
        listeners.erase(value);
        return true;
    } else
        logger->error("Attempt to delete listener but he doesn't exist(listener (addr){})",
                      value);
    return false;
}
