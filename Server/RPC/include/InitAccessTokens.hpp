#pragma once

#include <chrono>
#include <format>
#include <optional>
#include <thread>
#include <unordered_map>

#include <rpc/client.h>
#include <spdlog/spdlog.h>
#include <subprocess.hpp>

#include "Configuration.hpp"
#include "Globals.hpp"
#include "Randomgen.hpp"
#include "RPCErrorConverter.hpp"
#include "RPCServer.hpp"
#include "Macros.hpp"
#include "TerminateHandler.hpp"

static inline std::unordered_map<std::string, std::pair<std::string, std::string>> access_tokens;

inline const std::unordered_map<std::string, std::pair<std::string, std::string>>& get_access_tokens() { return access_tokens; }

inline void init_access_tokens(const std::vector<config::BaseModuleInfo>& el_modules) {
    static auto logger = std::make_shared<spdlog::logger>("DML", spdlog::sinks_init_list{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() });
    spdlog::initialize_logger(logger);

    std::optional<config::BaseModuleInfo> main_module = std::nullopt;
    for (auto& el_module : el_modules) {
        if (!std::filesystem::exists(el_module.file))
            shutdown(EXIT_FAILURE, std::format("Failed to load early-loading module: file '{}' does not exist",
                el_module.file.string()));

        if (el_module.main) {
            if (!main_module.has_value()) {
                spdlog::info("{}: Found the main module: {}",
                    FUNCSIG, el_module.file.string());
                main_module = el_module;
            } else
                spdlog::warn("{}: Found the new main module but it already exists; skipping",
                    FUNCSIG);
            continue;
        }

        globals::detached_processes.push_back(subprocess::RunBuilder({el_module.prefix, el_module.file.string()}).popen());
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::optional<rpc::client> el_module_rpc_test;
        try {
            el_module_rpc_test.emplace("127.0.0.1", el_module.rpc_port);
        } catch (rpc::rpc_error& rpcerr) {
            shutdown(EXIT_FAILURE, std::format("Failed to load early-loading module '{}': rpc::client throws an exception(state: init_client): {}",
                el_module.file.string(), convert_rpc_error(rpcerr)));
        }

        auto generated_access_token = gen_access_token();
        try {
            el_module_rpc_test->call(el_module.set_access_token_function, generated_access_token);
        } catch (rpc::rpc_error& rpcerr) {
            shutdown(EXIT_FAILURE, std::format("Failed to load early-loading module '{}': rpc::client throws an exception(state: call): {}",
                el_module.file.string(), convert_rpc_error(rpcerr)));
        }

        access_tokens[generated_access_token] = std::make_pair(el_module.mod_id, el_module.file.string());
    }

    if (main_module) {
        auto value = main_module.value();
        logger->info("Loading the main module '{}'",
            value.file.string());
        server::rpc::up_rpc_server();

        subprocess::Popen main_module_process;
        if (value.prefix.empty())
            main_module_process = subprocess::Popen({value.file.string()}, {});
        else
            main_module_process = subprocess::Popen({value.prefix, value.file.string()}, {});

        rpc::client main_module_rpc("127.0.0.1", value.rpc_port);
        auto generated_access_token = gen_access_token();
        access_tokens[generated_access_token] = std::make_pair(value.mod_id, value.file.string());
        try {
            main_module_rpc.call(value.set_access_token_function, generated_access_token);
        } catch (rpc::rpc_error& rpcerr) {
            shutdown(EXIT_FAILURE, std::format("The main module throws an exception: {}",convert_rpc_error(rpcerr)));
        }

        while (!main_module_process.poll()) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }

        shutdown(0, "Main module proccess is ended; exit!");
    }
}