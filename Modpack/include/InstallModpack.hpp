#pragma once

#include <chrono>
#include <filesystem>
#include <map>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Globals.hpp"
#include "ModpackRead.hpp"
#include "Macros.hpp"

namespace modpack {
    template <template <typename, typename> class MapType> bool install_modpack(ModpackReader&& modpack_obj) {
        static auto logger = std::make_shared<spdlog::logger>("DUSK::install_modpack", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
        std::expected<ModpackReader::ValuesMap<MapType>, std::string> result;
        if (modpack_obj.get_extracted_directory().empty()) {
            auto extract_result = modpack_obj.extract_modpack(std::filesystem::path(DUSK_TMP) / std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
            if (!extract_result.has_value()) {
                logger->error("Failed to extract modpack: libarchive-cpp: {}",
                    extract_result.error());
                return false;
            }
        }

        result = modpack_obj.get_values_of<MapType>("config.json");
        if (!result) {
            logger->error("Failed to get values of {}/config.json: {}",
                              modpack_obj.get_extracted_directory().string(), result.error());
            return false;
        }
        auto result_val = result.value();

        std::error_code err_code;
	    auto target_dir_name = [&modpack_obj] -> std::string { for (auto& dir : std::filesystem::directory_iterator(modpack_obj.get_extracted_directory())) { if (dir.is_directory()) return dir.path().stem().string(); } return ""; }();
        auto target_dir_path = globals::configuration->current_user.modules_directory() / target_dir_name;
        std::filesystem::create_directory(target_dir_path);
        auto platform = (result_val.contains("platform") ? result_val.at("platform").second : "");

	if (!platform.empty() && platform != "\"any\"") {
        logger->info("Installing modpack for platform {} from {} to {}",
            platform, modpack_obj.get_extracted_directory().string(), target_dir_path.string());
		if (platform == "\"windows\"") {
#if defined(OS_WINDOWS)
            std::filesystem::copy(modpack_obj.get_extracted_directory() / target_dir_name, target_dir_path, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
#else
			logger->warn("Specified platform is {} but current platform is Windows",
                platform);
#endif
		} else if (platform == "\"linux\"") {
#if defined(OS_LINUX)
            std::filesystem::copy(modpack_obj.get_extracted_directory() / target_dir_name, target_dir_path, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
#else
			logger->warn("Specified platform is {} but current platform is Linux",
                platform);
#endif
		} else if (platform == "\"android\"") {
#if defined(OS_ANDROID)
            std::filesystem::copy(modpack_obj.get_extracted_directory() / target_dir_name, target_dir_path, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
#else
			logger->warn("Specified platform is {} but current platform is Android",
                platform);
#endif
		}

    } else {
            logger->warn("Platform is not specified; install anyway");
            std::filesystem::copy(modpack_obj.get_extracted_directory() / target_dir_name, target_dir_path, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
    }

        if (err_code) {
            logger->error("Error when copying modpack: {}",
                err_code.message());
            return false;
        } else {
            logger->info("Modpack {} was installed",
                target_dir_name);
            return true;
        }
    }

    template <template <typename, typename> class MapType> bool install_modpack(const std::filesystem::path& modpack) {
        return install_modpack<MapType>(std::move(ModpackReader(modpack)));
    }
}
