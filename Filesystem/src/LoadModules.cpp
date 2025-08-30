#include "Configuration.hpp"

#include <chrono>
#include <optional>
#include <thread>
#include <unordered_map>

#include <rpc/client.h>
#include <rpc/rpc_error.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <subprocess.hpp>

#include "Globals.hpp"
#include "RPCErrorConverter.hpp"
#include "Macros.hpp"

std::optional<config::Module> load_module_info(const nlohmann::json& module_info_json) {
    config::Module module_obj;

    if (!module_info_json.contains("name")) {
        spdlog::warn("{}: Incorrect module config(!module.contains(\"name\")); module dump:\n{}",
                     FUNCSIG, module_info_json.dump(4));
        return std::nullopt;
    }
    module_obj.name = module_info_json.at("name").template get<std::string>();

    if (!module_info_json.contains("id")) {
        spdlog::warn("{}: Incorrect module config(!module.contains(\"id\")); module dump:\n{}",
                     FUNCSIG, module_info_json.dump(4));
        return std::nullopt;
    }
    module_obj.id = module_info_json.at("id").template get<std::string>();

    if (!module_info_json.contains("author")) {
        spdlog::warn(R"({}: Incomplete module config(!module.contains("author")); THIS IS NOT AN ERROR default value is "UNKNOWN")",
                     FUNCSIG);
        module_obj.author = "UNKNOWN";
    } else
        module_obj.author = module_info_json.at("author").template get<std::string>();

    if (!module_info_json.contains("version")) {
        spdlog::warn(R"({}: Incomplete module config(!module.contains("version")); THIS IS NOT AN ERROR default value is "0.0.0")",
            FUNCSIG);
        module_obj.version = "0.0.0";
    } else
        module_obj.version = module_info_json.at("version").template get<std::string>();

    if (!module_info_json.contains("description")) {
        spdlog::warn(R"({}: Incomplete module config(!module.contains("description")); THIS IS NOT AN ERROR default value is "")",
            FUNCSIG);
        module_obj.description = "";
    } else
        module_obj.description = module_info_json.at("description").template get<std::string>();

    return module_obj;
}

void config::UserConfiguration::load_modules() {
    for (const auto& module : this->modules_base_) {
        globals::detached_processes.push_back(subprocess::RunBuilder({module.prefix, module.file.string()}).popen());
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::optional<rpc::client> module_rpc_test; // <-- std::optional for delayed initialization
        try {
            module_rpc_test.emplace("127.0.0.1", module.rpc_port);
        } catch (rpc::rpc_error& rpcerr) {
            spdlog::error("{}: Failed to load module '{}': rpc::client throws an exception(state: init_client): {}",
                FUNCSIG, module.file.string(), convert_rpc_error(rpcerr));
            continue;
        }

        if (!module_rpc_test) {
            spdlog::error("{}: Failed to load module '{}': rpc::client is not initialized",
                FUNCSIG, module.file.string());
            continue;
        }

        clmdep_msgpack::object_handle module_config;
        try {
            module_config = module_rpc_test->call(module.get_config_rpc_function);
        } catch (rpc::rpc_error& rpcerr) {
            spdlog::error("{}: Failed to load module '{}': rpc::client throws an exception(state: call): {}",
                FUNCSIG, module.file.string(), convert_rpc_error(rpcerr));
        }
        nlohmann::json module_config_json;
        try {
            module_config_json = nlohmann::json::parse(module_config.as<std::string>());
            spdlog::debug("{}: Dump of the module response:\n{}:\n{}",
                FUNCSIG, module.get_config_rpc_function, module_config_json.dump(4));
        } catch (rpc::rpc_error& rpcerr) {
            spdlog::error("{}: Failed to load module '{}': rpc::client throws an exception(state: parse_response): {}",
                FUNCSIG, module.file.string(), convert_rpc_error(rpcerr));
            continue;
        }

        if (auto module_config_obj = load_module_info(module_config_json); module_config_obj)
            this->modules_.push_back(std::move(module_config_obj.value()));
        else
            spdlog::warn("{}: Failed to load module '{}': load_module_info returned invalid object(check previous logs)",
                FUNCSIG, module.file.string());
    }

    spdlog::info("{}: Loaded {} modules",
        FUNCSIG, this->modules_.size());
    spdlog::debug("{}: Detached {} processes",
        FUNCSIG, globals::detached_processes.size());
}
