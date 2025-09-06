#pragma once

#include <algorithm>
#include <exception>
#include <sstream>
#include <stacktrace>
#include <thread>

#include <rpc/rpc_error.h>
#include <spdlog/spdlog.h>

#include "FS.hpp"
#include "Globals.hpp"
#include "RPCServer.hpp"
#include "Macros.hpp"
#include "Updates.hpp"

inline void shutdown(int rcode, const std::string& message = "") noexcept {
    /* Get and print stacktrace */
    std::ostringstream stacktrace;
    stacktrace << std::stacktrace::current();
    spdlog::info("{}: Stacktrace:\n{}",
        FUNCSIG, stacktrace.str());


    if (!message.empty())
        spdlog::info("{}: message: {}",
            FUNCSIG, message);

    /* RPC server finalizing */
    if (globals::rpc_server)
        server::rpc::shutdown_rpc_server();

    /* Clean env (remove the DUSK_TMP folder and more) */
    filesystem::clean_env();

    /* Kill all detached processes(like modules) */
    std::ranges::for_each(globals::detached_processes, [](auto& process) { process.kill(); });

    /* Close a tdlib instance */
    std::thread close_tdlib_instance_th([] { update::send_request(td::td_api::make_object<td::td_api::close>()); });

    /* ^
       | Waiting for tdlib instance closing */
    spdlog::info("{}: Closing tdlib instance...",
    FUNCSIG);
    close_tdlib_instance_th.join();

    /* Close loggers */
    spdlog::shutdown();

    std::exit(rcode);
}

inline void terminate_handler_f() {
    auto current_exc = std::current_exception();
    try {
        std::rethrow_exception(current_exc);
    }
    catch (rpc::rpc_error& rpc_error) {
        auto object = rpc_error.get_error().as<RPCLIB_MSGPACK::object>();
        std::ostringstream error_dump;
        error_dump << object;
        spdlog::critical("{}: RPC server exception: {}: {}",
            FUNCSIG, rpc_error.get_function_name(), error_dump.str());
    } catch (std::exception& error) {
        spdlog::critical("{}: Exception: {}",
            FUNCSIG, error.what());
    } catch (...) {
        spdlog::critical("{}: Exception: UNKNOWN");
    }

    shutdown(3);
}

inline void signal_handler_f(int signal) {
    shutdown(signal, "Called the signal handler with signal " + std::to_string(signal));
}