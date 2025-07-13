#pragma once

#include <exception>
#include <spdlog/spdlog.h>

#include "FS.hpp"
#include "RPCServer.hpp"
#include "Macros.hpp"

inline void shutdown(int rcode, const std::string& message = "") noexcept {
    if (!message.empty())
        spdlog::info("{}: {}",
            "Shutdown", message);
    server::rpc::shutdown_rpc_server();
    filesystem::clean_env();
    spdlog::shutdown();
    std::exit(rcode);
}

inline void terminate_handler_f() {
    auto current_exc = std::current_exception();
    try {
        std::rethrow_exception(current_exc);
    } catch (std::exception& error) {
        spdlog::error("{}: Exception: {}",
                      FUNCSIG, error.what());
    } catch (...) {
        spdlog::error("{}: Exception: UNKNOWN");
    }

    filesystem::clean_env();
    std::exit(3);
}