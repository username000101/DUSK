#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include <td/telegram/Client.h>

namespace auth {
    bool setTdlibParameters(std::shared_ptr<td::ClientManager> client,
                            bool use_test_dc,
                            std::filesystem::path db_dir,
                            std::filesystem::path files_dir,
                            bool use_files_db,
                            bool use_chat_info_db,
                            bool use_message_db,
                            bool use_secret_chats,
                            td::td_api::int32 api_id,
                            std::string api_hash,
                            std::string system_lang_code,
                            std::string device_model,
                            std::string system_version,
                            std::string application_version);
    bool authorize();
}
