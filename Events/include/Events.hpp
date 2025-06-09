#pragma once

#include <chrono>
#include <cstdint>
#include <stack>
#include <unordered_map>
#include <utility>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <td/telegram/td_api.h>

namespace events {
    using EventSignature = td::td_api::object_ptr<td::td_api::Update>;
    using ModifiedEventSignature = std::shared_ptr<td::td_api::Update>;
    using EventListenerSignature = bool(*)(ModifiedEventSignature);
    enum EventType { UPDATE_NEW_MESSAGE, UPDATE_EDIT_MESSAGE, };

    class EventsInteractions {
    private:
        static inline auto logger = std::make_shared<spdlog::logger>("libevents", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});

        static inline std::stack<std::pair<EventType, EventSignature>> updates;
        static inline std::unordered_map<std::uintptr_t, EventListenerSignature> listeners;

        static inline bool broadcast_stop;
    public:
        static bool append_listener(EventListenerSignature listener);
        static bool remove_listener(EventListenerSignature listener);
        static void automatic_broadcaster(std::chrono::milliseconds sleep_time);
        static void append_update(td::td_api::object_ptr<td::td_api::Update> update);
        static void broadcaster_stop() { broadcast_stop = true; }
    };
}
