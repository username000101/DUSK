#include "Auth.hpp"

#include <filesystem>

#include <td/telegram/td_api.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Globals.hpp"
#include "Updates.hpp"

bool auth::setTdlibParameters(std::shared_ptr<td::ClientManager> client, bool use_test_dc, bool use_files_db, bool use_chat_info_db, bool use_message_db, bool use_secret_chats, td::td_api::int32 api_id, std::string api_hash, std::string system_lang_code, std::string device_model, std::string system_version, std::string application_version) {
    static auto logger = std::make_shared<spdlog::logger>("Auth::setTdlibParameters", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
    spdlog::initialize_logger(logger);

    if (!client)
        return false;

    auto db_dir = std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user) / "database";
    auto files_dir = std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user) / "files";

    if (!std::filesystem::exists(db_dir)) {
        logger->warn("Creating unexisting database directory: {}",
                     db_dir.string());
        std::filesystem::create_directory(db_dir);
    }
    if (!std::filesystem::exists(files_dir)) {
        logger->warn("Creating unexisting files directory: {}",
                      files_dir.string());
        std::filesystem::create_directory(files_dir);
    }

    auto parameters = td::td_api::make_object<td::td_api::setTdlibParameters>();
    parameters->api_hash_ = api_hash;
    parameters->api_id_ = api_id;
    parameters->application_version_ = application_version;
    parameters->device_model_ = device_model;
    parameters->system_language_code_ = system_lang_code;
    parameters->system_version_ = system_version;
    parameters->use_chat_info_database_ = use_chat_info_db;
    parameters->use_file_database_ = use_files_db;
    parameters->use_message_database_ = use_message_db;
    parameters->use_secret_chats_ = use_secret_chats;
    parameters->database_directory_ = db_dir.string();
    parameters->files_directory_ = files_dir.string();
    parameters->use_test_dc_ = use_test_dc;

    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));

    auto response = update::send_request(std::move(parameters), client, client->create_client_id());

    return false;
}
