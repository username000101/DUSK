#pragma once

/* TODO:
 * Make functions for all privileged operations
 * and just use one functions for check access token
 */

#include <cstdint>
#include <thread>

#include <rpc/rpc_error.h>
#include <rpc/server.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Events.hpp"
/* Commands */
#include "CommandEditTextMessage.hpp"
#include "CommandsEventsHandling.hpp"
#include "CommandModule.hpp"
#include "CommandInit.hpp"
#include "CommandSendTextMessage.hpp"
#include "CommandSetUser.hpp"
#include "CommandVersion.hpp"
/* -------- */
#include "Globals.hpp"
#include "RPCErrorConverter.hpp"

extern void shutdown(int rcode, const std::string& message) noexcept;

static std::shared_ptr<spdlog::logger> logger = nullptr;

namespace server {
    namespace rpc {
        inline void up_rpc_server(std::uint16_t port = 5000) {
            if (!logger) {
                logger = std::make_shared<spdlog::logger>("RPC Server", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
                spdlog::initialize_logger(logger);
            }

            if (globals::rpc_server) {
                logger->info("Server already started");
                return;
            }

            events::EventsInteractions::append_listener(td::td_api::updateNewMessage::ID, process_update);

            globals::rpc_server = std::make_shared<::rpc::server>(port);

            globals::rpc_server->bind("dusk.set_event_handler", dusk_rpc_server_command_set_event_handler);
            globals::rpc_server->bind("dusk.version", dusk_rpc_server_command_version);
            globals::rpc_server->bind("dusk.module", dusk_rpc_server_command_module);
            globals::rpc_server->bind("dusk.init", dusk_rpc_server_command_init);
            globals::rpc_server->bind("dusk.send_text_message", dusk_rpc_server_command_send_text_message);
            globals::rpc_server->bind("dusk.set_user", dusk_rpc_server_command_set_user);
            globals::rpc_server->bind("dusk.edit_text_message", dusk_rpc_server_command_edit_text_message);

            std::thread rpc_server_thread([] { try { globals::rpc_server->run(); } catch (::rpc::rpc_error& rpcerr) { std::cout << "EXCEPTION: "<< convert_rpc_error(rpcerr) << std::endl; } });
            logger->info("RPC server has been started on port {}",
                port);
            rpc_server_thread.detach();
        }

        inline void shutdown_rpc_server() {
            if (!globals::rpc_server)
                logger->warn("Failed to shutdown rpc server: structure is not initialized");
            else
                globals::rpc_server->stop();
        }
    }
}