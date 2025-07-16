#include "Configuration.hpp"

#include <chrono>
#include <optional>
#include <thread>
#include <unordered_map>

#include <rpc/client.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <subprocess.hpp>

#include "Globals.hpp"
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
        spdlog::warn("{}: Incomplete module config(!module.contains(\"author\")); THIS IS NOT AN ERROR default value is \"UNKNOWN\"",
                     FUNCSIG);
        module_obj.author = "UNKNOWN";
    } else
        module_obj.author = module_info_json.at("author").template get<std::string>();

    if (!module_info_json.contains("version")) {
        spdlog::warn("{}: Incomplete module config(!module.contains(\"version\")); THIS IS NOT AN ERROR default value is \"0.0.0\"",
            FUNCSIG);
        module_obj.version = "0.0.0";
    } else
        module_obj.version = module_info_json.at("version").template get<std::string>();

    if (!module_info_json.contains("description")) {
        spdlog::warn("{}: Incomplete module config(!module.contains(\"description\")); THIS IS NOT AN ERROR default value is \"\"",
            FUNCSIG);
        module_obj.description = "";
    } else
        module_obj.description = module_info_json.at("description").template get<std::string>();

    return module_obj;
}

const static std::unordered_map<std::string, std::string> prefix_table = {
    {".py", "python"},
    {".php", "php"},
    {".ruby", "ruby"}
};

void config::UserConfiguration::load_modules() {
    for (const auto& module : this->modules_base_) {
        std::string prefix = "";
        if (prefix_table.contains(module.file.extension().string()))
            prefix = prefix_table.at(module.file.extension().string());
        globals::detached_processes.push_back(subprocess::RunBuilder({prefix, module.file.string()}).popen());
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::shared_ptr<rpc::client> module_rpc_test;

        try {
            module_rpc_test = std::make_shared<rpc::client>("127.0.0.1", module.rpc_port);
        } catch (std::exception& e) {
            spdlog::error("{}: Failed to load module '{}': rpc::client throws an exception(state: init_client): {}",
                FUNCSIG, module.file.string(), e.what());
            continue;
        }

        if (!module_rpc_test) {
            spdlog::error("{}: Failed to load module '{}': rpc::client is not initialized",
                FUNCSIG, module.file.string());
            continue;
        }

        auto module_config = module_rpc_test->call(module.get_config_rpc_function);
        nlohmann::json module_config_json;
        try {
            module_config_json = nlohmann::json::parse(module_config.as<std::string>());
            spdlog::debug("{}: Dump of the module response:\n{}:\n{}",
                FUNCSIG, module.get_config_rpc_function, module_config_json.dump(4));
        } catch (std::exception& e) {
            spdlog::error("{}: Failed to load module '{}': rpc::client throws an exception(state: parse_response): {}",
                FUNCSIG, module.file.string(), e.what());
            continue;
        }

        auto module_config_obj = load_module_info(module_config_json);
        if (module_config_obj)
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
