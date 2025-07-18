#pragma once

#include <cstdint>
#include <string>

#include <td/telegram/td_api.h>

#include "Updates.hpp"

inline std::string dusk_rpc_server_command_edit_text_message(std::int64_t chat_id, std::int64_t message_id, const std::string& text) {
    auto edit_msg_obj = td::td_api::make_object<td::td_api::editMessageText>();
    edit_msg_obj->chat_id_ = chat_id;
    edit_msg_obj->message_id_ = message_id;

    auto new_msg_text_obj = td::td_api::make_object<td::td_api::inputMessageText>();
    auto formatted_text = td::td_api::make_object<td::td_api::formattedText>();
    formatted_text->text_ = text;
    new_msg_text_obj->text_ = std::move(formatted_text);

    edit_msg_obj->input_message_content_ = std::move(new_msg_text_obj);

    auto response = update::send_request(std::move(edit_msg_obj));
    if (response.object)
        return td::td_api::to_string(response.object);
    else
        return "";
}