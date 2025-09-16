#include "Events.hpp"

bool events::EventsInteractions::remove_listener(events::EventListenerSignature listener) {
    if (!logger) {
        logger = std::make_shared<spdlog::logger>("Events", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
        spdlog::initialize_logger(logger);
    }

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
