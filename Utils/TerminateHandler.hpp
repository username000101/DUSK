#pragma once

#include <algorithm>
#include <exception>
#include <spdlog/spdlog.h>

#include "FS.hpp"
#include "Globals.hpp"
#include "RPCServer.hpp"
#include "Macros.hpp"

inline void shutdown(int rcode, const std::string& message = "") noexcept {
    if (!message.empty())
        spdlog::info("{}: {}",
            FUNCSIG, message);
    server::rpc::shutdown_rpc_server();
    filesystem::clean_env();
    std::ranges::for_each(globals::detached_processes, [](auto& process) { process.kill(); });
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

    shutdown(3);
}

inline void signal_handler_f(int signal) {
    shutdown(signal, "Called the signal handler with signal " + std::to_string(signal));
}