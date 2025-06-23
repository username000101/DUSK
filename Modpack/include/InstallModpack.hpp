#pragma once

#include <chrono>
#include <map>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Globals.hpp"
#include "ModpackRead.hpp"
#include "Macros.hpp"

namespace modpack {
    template <template <typename, typename> class MapType> bool install_modpack(ModpackReader&& modpack_obj) {
        ModpackReader::ValuesMap<MapType> result;
        if (modpack_obj.get_extracted_directory().empty())
            modpack_obj.extract_modpack(std::filesystem::path(DUSK_TMP) / std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        try {
            result = modpack_obj.get_values_of<std::map>("config.json");
        } catch (std::exception& error) {
            spdlog::error("{}: Failed to get values of {}/config.json: {}",
                          FUNCSIG, modpack_obj.get_extracted_directory().string(), error.what());
            return false;
        }

        if (result.contains("prebuilt")) {
            std::string prebuilt_str = result.at("prebuilt").second;
            std::filesystem::path current_platform_library, source_path, target_path;
            auto prebuilt = nlohmann::json::parse(prebuilt_str);
#if defined(__linux__)
            if (!prebuilt.contains("linux")) {
                spdlog::error("{}: Not found prebuilt libs for current(linux) platform",
                              FUNCSIG);
                return false;
            }
            current_platform_library = modpack_obj.get_extracted_directory() / prebuilt.at("linux").template get<std::string>();
            target_path = std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user) / "modules" / current_platform_library.filename();
            source_path = modpack_obj.get_extracted_directory() / current_platform_library;
            if (!std::filesystem::exists(source_path)) {
                spdlog::error("{}: Library for current platform(linux) is set but doesn't exist({})",
                              FUNCSIG, current_platform_library.string());
                return false;
            }
#elif defined(_WIN32)
            if (!prebuilt.contains("windows")) {
                spdlog::error("{}: Not found prebuilt libs for current(windows) platform",
                              FUNCSIG);
                return false;
            }
            current_platform_library = prebuilt.at("windows").get<std::filesystem::path>();
            target_path = std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user) / "modules" / current_platform_library.filename();
            source_path = modpack_obj.get_extracted_directory() / current_platform_library;
            if (!std::filesystem::exists(source_path)) {
                spdlog::error("{}: Library for current platform(windows) is set but doesn't exist({})",
                              FUNCSIG, current_platform_library.string());
                return false;
            }
#else
#error "Current platform is not supported now"
#endif
            spdlog::info("{}: Copying library {}",
                         FUNCSIG, current_platform_library.string());
            std::filesystem::copy(source_path, target_path);
            return true;
        }
        return false;
    }

    template <template <typename, typename> class MapType> bool install_modpack(const std::filesystem::path& modpack) {
        return install_modpack<MapType>(ModpackReader(modpack));
    }
}