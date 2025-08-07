#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <utility>

#include <nlohmann/json.hpp>

namespace modpack {
    class   ModpackReader {
    public:
        ModpackReader(std::filesystem::path modpack_path) : file_(modpack_path) {
            if (!std::filesystem::exists(modpack_path.string()) || std::filesystem::is_directory(modpack_path.string()))
                throw std::runtime_error("Cannot open modpack file");
        }

        std::expected<std::filesystem::path, std::string> extract_modpack(std::filesystem::path into);

        enum ParameterType { STRING, INT, OBJECT, ARRAY, };
        template <template <typename, typename> class MapType> using ValuesMap = MapType<std::string, std::pair<ParameterType, std::string>>;

        template <template <typename, typename> class MapType> std::expected<ValuesMap<MapType>, std::string> get_values_of(std::filesystem::path config_file) {
            if (!std::filesystem::exists(config_file) || std::filesystem::is_directory(config_file)) {
                if (!std::filesystem::exists(this->extracted_modpack_path_ / config_file) || std::filesystem::is_directory(config_file))
                    return std::unexpected("File does not exist");
                config_file = this->extracted_modpack_path_ / config_file;
            }

            std::ifstream config_file_stream(config_file);
            if (!config_file_stream.is_open())
                return std::unexpected("Could not open config file");
            nlohmann::json config = nlohmann::json::parse(config_file_stream);
            config_file_stream.close();
            ValuesMap<MapType> result;

            for (auto element = config.begin(); element != config.end(); ++element) {
                if (element.value().is_string())
                    result[element.key()] = {ParameterType::STRING, element.value().get<std::string>()};
                if (element.value().is_object())
                    result[element.key()] = {ParameterType::OBJECT, element.value().dump()};
                else if (element.value().is_array())
                    result[element.key()] = {ParameterType::ARRAY, element.value().dump()};
                else if (element.value().is_number())
                    result[element.key()] = {ParameterType::INT, std::to_string(element.value().get<int>())};
                else
                    result[element.key()] = {ParameterType::STRING, element.value().dump()};
            }

            return result;
        }

        std::filesystem::path get_extracted_directory() { return this->extracted_modpack_path_; }

    private:
        std::filesystem::path file_, extracted_modpack_path_;
    };
}