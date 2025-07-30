#pragma once

#include <cstdint>
#include <string>

#include <glaze/glaze.hpp>
#include <td/telegram/td_api.h>

#include "Updates.hpp"

inline std::string dusk_rpc_server_command_send_text_message(std::int64_t chat_id, const std::string& text, const std::int64_t reply_to_msg_id) {
    auto send_msg_obj = td::td_api::make_object<td::td_api::sendMessage>();
    send_msg_obj->chat_id_ = chat_id;

    auto msg_content_obj = td::td_api::make_object<td::td_api::inputMessageText>();
    auto formatted_text_obj = td::td_api::make_object<td::td_api::formattedText>();
    formatted_text_obj->text_ = text;
    msg_content_obj->text_ = std::move(formatted_text_obj);

    send_msg_obj->input_message_content_ = std::move(msg_content_obj);

    if (reply_to_msg_id != 0) {
        auto msg_reply_to_obj = td::td_api::make_object<td::td_api::inputMessageReplyToMessage>();
        msg_reply_to_obj->message_id_ = reply_to_msg_id;
        send_msg_obj->reply_to_ = std::move(msg_reply_to_obj);
    }

    auto response = update::send_request(std::move(send_msg_obj));
    if (!response.object)
        return "ERROR/INVALID OBJECT";
    if (response.object->get_id() == td::td_api::error::ID) {
        const auto* error_details = static_cast<const td::td_api::error*>(response.object.get());
        return error_details->message_;
    }
    return glz::write_json(*static_cast<td::td_api::message*>(response.object.get())).value_or("ERROR");


}
