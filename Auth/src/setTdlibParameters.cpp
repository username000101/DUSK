#include "Auth.hpp"

#include <iostream>

#include <td/telegram/td_api.h>

#include "Updates.hpp"

bool auth::setTdlibParameters(std::shared_ptr<td::ClientManager> client, bool use_test_dc, std::filesystem::path db_dir, std::filesystem::path files_dir, bool use_files_db, bool use_chat_info_db, bool use_message_db, bool use_secret_chats, td::td_api::int32 api_id, std::string api_hash, std::string system_lang_code, std::string device_model, std::string system_version, std::string application_version) {
    if (!client)
        return false;
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

    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(3));

    auto response = update::send_request(std::move(parameters), client, client->create_client_id());
    std::cout << "response id is " << (response.object ? std::to_string(response.object->get_id()) : "NULLPTR") << std::endl;

    return false;
}
