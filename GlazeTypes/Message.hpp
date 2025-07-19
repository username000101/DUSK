#pragma once

#include <chrono>
#include <cstdint>
#include <locale>
#include <sstream>

#include <glaze/glaze.hpp>
#include <td/telegram/td_api.h>
#include <spdlog/spdlog.h>

template <>
struct glz::meta<td::td_api::message> {
    using T = td::td_api::message;
    static constexpr auto value = glz::object(
        "id", &T::id_,
        "sender", [] (const T& self) -> std::string {
            if (!self.sender_id_)
                return "ERROR";
            if (self.sender_id_->get_id() == td::td_api::messageSenderChat::ID) {
                const auto* sender_chat = static_cast<const td::td_api::messageSenderChat*>(self.sender_id_.get());
                return std::to_string(sender_chat->chat_id_);
            } else {
                const auto* sender_user = static_cast<const td::td_api::messageSenderUser*>(self.sender_id_.get());
                return std::to_string(sender_user->user_id_);
            }
        },
        "chat_id", &T::chat_id_,
        "sending_state", [] (const T& self) -> std::string {
            if (!self.sending_state_)
                return "ERROR";
            if (self.sending_state_->get_id() == td::td_api::messageSendingStateFailed::ID) {
                const auto* state_failed = static_cast<const td::td_api::messageSendingStateFailed*>(self.sending_state_.get());
                const auto* error = static_cast<const td::td_api::error*>(state_failed->error_.get());
                return "failed: " + error->message_;
            } else {
                const auto* state_pending = static_cast<const td::td_api::messageSendingStatePending*>(self.sending_state_.get());
                return std::to_string(state_pending->sending_id_);
            }
        },
        "scheduling_state", "not implemented",
        "is_outgoing", &T::is_outgoing_,
        "is_pinned", &T::is_pinned_,
        "is_from_offile", &T::is_from_offline_,
        "can_be_saved", &T::can_be_saved_,
        "has_timestamped_media", &T::has_timestamped_media_,
        "is_channel_post", &T::is_channel_post_,
        "contains_unread_mention", &T::contains_unread_mention_,
        "date", &T::date_,
        "edit_date", &T::edit_date_
    );
};