#pragma once

#include <chrono>
#include <cstdint>
#include <locale>
#include <optional>
#include <sstream>

#include <glaze/glaze.hpp>
#include <td/telegram/td_api.h>
#include <spdlog/spdlog.h>

struct __impl_message_origin {
    std::int64_t chat_or_sender_id = 0;
    std::optional<std::int64_t> message_id = std::nullopt;
    std::optional<std::string> sender_name = std::nullopt;
    std::optional<std::string> author_signature = std::nullopt;
};

struct __impl_forwardInfo {
    std::int32_t date = 0;
    std::pair<std::int32_t, __impl_message_origin> origin;
};

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
        "scheduling_state", "ERROR",
        "is_outgoing", &T::is_outgoing_,
        "is_pinned", &T::is_pinned_,
        "is_from_offile", &T::is_from_offline_,
        "can_be_saved", &T::can_be_saved_,
        "has_timestamped_media", &T::has_timestamped_media_,
        "is_channel_post", &T::is_channel_post_,
        "contains_unread_mention", &T::contains_unread_mention_,
        "date", &T::date_,
        "edit_date", &T::edit_date_,
        "forward_info", [] (const T& self) -> std::string {
            if (!self.forward_info_)
                return "ERROR";
            const auto* fwd_info = static_cast<const td::td_api::messageForwardInfo*>(self.forward_info_.get());
            std::int32_t result_origin_type = 0;
            __impl_message_origin origin;
            switch (fwd_info->origin_->get_id()) {
                case td::td_api::messageOriginChat::ID: {
                    const auto* origin_chat = static_cast<const td::td_api::messageOriginChat*>(fwd_info->origin_.get());
                    origin.author_signature = origin_chat->author_signature_;
                    origin.chat_or_sender_id = origin_chat->sender_chat_id_;
                    result_origin_type = td::td_api::messageOriginChat::ID;
                    break;
                }
                case td::td_api::messageOriginChannel::ID: {
                    const auto* origin_channel = static_cast<const td::td_api::messageOriginChannel*>(fwd_info->origin_.get());
                    origin.author_signature = origin_channel->author_signature_;
                    origin.chat_or_sender_id = origin_channel->chat_id_;
                    origin.message_id = origin_channel->message_id_;
                    result_origin_type = td::td_api::messageOriginChannel::ID;
                    break;
                }
                case td::td_api::messageOriginHiddenUser::ID: {
                    const auto* hidden_user = static_cast<const td::td_api::messageOriginHiddenUser*>(fwd_info->origin_.get());
                    origin.sender_name = hidden_user->sender_name_;
                    result_origin_type = td::td_api::messageOriginHiddenUser::ID;
                    break;
                }
                case td::td_api::messageOriginUser::ID: {
                    const auto* user = static_cast<const td::td_api::messageOriginUser*>(fwd_info->origin_.get());
                    origin.chat_or_sender_id = user->sender_user_id_;
                }
            }
            return glz::write_json(__impl_forwardInfo{.date = fwd_info->date_, .origin = std::make_pair(result_origin_type, origin)}).value_or("ERROR");
        },
        "import_info", "ERROR",
        "interaction_info", "ERROR",
        "content", [] (const T& self) -> std::pair<std::int32_t, std::string> {
            if (!self.content_)
                return std::make_pair(0, "ERROR");
            switch (self.content_->get_id()) {
                case td::td_api::messageText::ID: {
                    const auto* message_text = static_cast<const td::td_api::messageText*>(self.content_.get());
                    return std::make_pair(td::td_api::messageText::ID, message_text->text_->text_);
                }
                default:
                    return std::make_pair(1, "ERROR");
            }
        }
    );
};