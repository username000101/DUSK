#include "Callbacks.hpp"

#include <filesystem>

#include <spdlog/spdlog.h>

#include "ConfirmDialog.hpp"

void args::callbacks::remove_user(const std::int64_t& user_chat_id) {
    std::filesystem::path full_user_data_path = std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(user_chat_id);
    if (auto diriter = std::filesystem::directory_iterator(full_user_data_path); diriter == std::filesystem::directory_iterator{}) {
        if (confirm(R"(Are you sure want to delete user ")" + std::to_string(user_chat_id) + R"(")")) {
            spdlog::warn(R"(Deleting user "{}")",
                         user_chat_id);
            std::filesystem::remove_all(full_user_data_path);
            std::exit(0);
        } else
            std::exit(1);
    } else {
        spdlog::warn(R"(User "{}" does not exist(search by user id "{}")",
                     user_chat_id, full_user_data_path.string());
        std::exit(2);
    }

}
