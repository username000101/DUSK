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
            std::string current_platform_library, prebuilt_str = result.at("prebuilt").second;
            auto prebuilt = nlohmann::json::parse(prebuilt_str);
#if defined(__linux__)
            if (!prebuilt.contains("linux")) {
                spdlog::error("{}: Not found prebuilt libs for current(linux) platform",
                              FUNCSIG);
                return false;
            }
            current_platform_library = modpack_obj.get_extracted_directory() / prebuilt.at("linux").template get<std::string>();
            if (!std::filesystem::exists(current_platform_library)) {
                spdlog::error("{}: Library for current platform(linux) is set but doesn't exist({})",
                              FUNCSIG, current_platform_library);
                return false;
            }
#elif defined(_WIN32)
            if (prebuilt.contains("windows")) {
                spdlog::error("{}: Not found prebuilt libs for current(linux) platform",
                              FUNCSIG);
                return false;
            }
            current_platform_library = prebuilt.at("windows").get<std::string>();
            if (!std::filesystem::exists(current_platform_library)) {
                spdlog::error("{}: Library for current platform(linux) is set but doesn't exist({})",
                              FUNCSIG, current_platform_library);
                return false;
            }
#else
#error "Current platform is not supported now"
#endif
            spdlog::info("{}: Copying library {}",
                         FUNCSIG, current_platform_library);
            std::filesystem::copy(current_platform_library, std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user) / "modules");
            return true;
        }
        return false;
    }

    template <template <typename, typename> class MapType> bool install_modpack(const std::filesystem::path& modpack) {
        return install_modpack<MapType>(ModpackReader(modpack));
    }
}