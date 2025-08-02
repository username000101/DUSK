#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>

#include <rpc/client.h>
#include <glaze/glaze.hpp>
#include <td/telegram/td_api.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Events.hpp"

#include "Message.hpp"
#include "UpdateChatLastMessage.hpp"

static inline std::unordered_map<std::int64_t, std::vector<std::pair<std::uint16_t, std::string>>> external_handlers;


inline bool process_update(events::ModifiedEventSignature update) {
    static auto logger = std::make_shared<spdlog::logger>("RPC::process_update", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
    if (!update)
        return false;
    if (external_handlers.contains(update->get_id())) {
        for (auto& handler : external_handlers.at(update->get_id())) {
            rpc::client client("127.0.0.1", handler.first);
            try {
                if (update->get_id() == td::td_api::updateNewMessage::ID) {
                    auto message = std::static_pointer_cast<td::td_api::updateNewMessage>(update);
                    auto msg = std::move(*message->message_.get());
                    client.async_call(handler.second, glz::write<glz::opts{.prettify = true}>(msg).value_or(""));
                }
            } catch (std::exception& rpcerr) {
                logger->error("rpc::client throw an exception: {}",
                    rpcerr.what());
            }
        }
    }
    return true;
}

inline std::string dusk_rpc_server_command_set_event_handler(std::int64_t event_id, const std::string& handler_function_name, std::uint16_t port) {
    static auto logger = std::make_shared<spdlog::logger>("RPC::set_event_handler", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});

    if (external_handlers.contains(event_id)) {
        bool has = false;
        std::ranges::for_each(external_handlers.at(event_id), [&has, &handler_function_name](auto& handler) { if (handler.second == handler_function_name) has = true; });
        if (has) {
            logger->warn("Attempt to overwrite the handler for event {} and name {}",
                event_id, handler_function_name);
            return "already_exists";
        }
    }
    external_handlers[event_id].push_back(std::make_pair(port, handler_function_name));
    logger->info("Setting handler for event {} with name {} from port {}",
    event_id, handler_function_name, port);
    return "";
}

inline std::string dusk_rpc_server_unset_event_handler(std::int64_t event_id, const std::string& handler_function_name) {
    if (external_handlers.contains(event_id)) {
        auto& handlers = external_handlers.at(event_id);
        for (auto iter = handlers.begin(); iter != handlers.end(); ++iter) {
            if (iter->second == handler_function_name) {
                handlers.erase(iter);
                break;
            }

        }
    }
}