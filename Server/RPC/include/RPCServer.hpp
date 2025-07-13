#pragma once

#include <cstdint>
#include <thread>

#include <rpc/server.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Globals.hpp"

static auto logger = std::make_shared<spdlog::logger>("RPC Server", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});

namespace server {
    namespace rpc {
        inline void up_rpc_server(std::uint16_t port = 5000) {
            globals::rpc_server = std::make_shared<::rpc::server>(port);
            std::thread rpc_server_thread([] { globals::rpc_server->run(); });
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