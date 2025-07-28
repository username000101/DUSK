#pragma once

#include <chrono>
#include <map>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Globals.hpp"
#include "ModpackRead.hpp"
#include "Macros.hpp"

enum Platform {
    Windows,
    Linux,
    MacOS,
    Android,
};

namespace modpack {
    template <template <typename, typename> class MapType> bool install_modpack(ModpackReader&& modpack_obj) {
        static auto logger = std::make_shared<spdlog::logger>("DUSK::install_modpack", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
        Platform platform;
        ModpackReader::ValuesMap<MapType> result;
        if (modpack_obj.get_extracted_directory().empty())
            modpack_obj.extract_modpack(std::filesystem::path(DUSK_TMP) / std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        try {
            result = modpack_obj.get_values_of<MapType>("config.json");
        } catch (std::exception& error) {
            logger->error("Failed to get values of {}/config.json: {}",
                          modpack_obj.get_extracted_directory().string(), error.what());
            return false;
        }

        if (!result.contains("platform")) {
            logger->error("Failed to install modpack: not found 'platform' section in config.json");
            return false;
        }

        if (auto platofrm_ = result.at("platform").second; platofrm_ == "windows")
            platform = Platform::Windows;
        else if (platofrm_ == "linux")
            platform = Platform::Linux;
        else if (platofrm_ == "macos")
            platform = Platform::MacOS;
        else if (platofrm_ == "android")
            platform = Platform::Android;
        else {
            logger->error("Failed to install modpack: invalid 'platform' section in config.json; available variants:[windows,linux,macos,android] but platform is '{}'",
                platofrm_);
            return false;
        }

        auto get_platform_string = [&platform] { switch (platform) { case Platform::Windows: return "Windows"; case Platform::Android: return "Android"; case Platform::Linux: return "Linux"; case Platform::MacOS: return "MacOS"; }};

        auto target_dir_name = [&modpack_obj] { for (auto& dir : std::filesystem::directory_iterator(modpack_obj.get_extracted_directory())) { if (dir.is_directory()) return dir.path().stem().string(); } }();
        auto traget_dir_path = globals::configuration->current_user.modules_directory() / target_dir_name;

        logger->debug("Copying modpack for platform {} in {}",
          get_platform_string(),  target_dir_name);

        std::filesystem::copy(modpack_obj.get_extracted_directory(), target_dir_name, std::filesystem::copy_options::recursive);
        return true;
    }

    template <template <typename, typename> class MapType> bool install_modpack(const std::filesystem::path& modpack) {
        try {
            return install_modpack<MapType>(ModpackReader(modpack));
        } catch (std::exception& error) {
            spdlog::warn("Failed to invoke install_modpack: {}", error.what());
            return false;
        }
    }
}