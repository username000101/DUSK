#pragma once

#include <chrono>
#include <cstdint>
#include <locale>
#include <sstream>

#include <glaze/glaze.hpp>
#include <td/telegram/td_api.h>
#include <spdlog/spdlog.h>

struct __impl_emojiStatus { std::int32_t expiration_date = 0; std::int64_t status_emoji_id = 0; };
struct __impl_verificationStatus { bool is_verified = false; bool is_scam = false; bool is_fake = false; std::int64_t bot_verification_icon_custom_emoji_id = 0; };

template <>
struct glz::meta<td::td_api::user> {
    using T = td::td_api::user;
    static constexpr auto value = glz::object(
        "id", &T::id_,
        "first_name", &T::first_name_,
        "last_name", &T::last_name_,
        "username", [] (const T& self) {
            auto result = [] (const T& self) -> std::variant<std::string, td::td_api::array<std::string>> {
                if (self.usernames_) {
                    const auto& unames = self.usernames_;
                    if (unames->active_usernames_.size() > 1)
                        return unames->active_usernames_;
                    return *unames->active_usernames_.begin();
                }
                return "None";
            }(self);

            if (std::holds_alternative<std::string>(result))
                return std::get<std::string>(result);
            else {
                std::vector<std::string> stdvec;
                for (auto& username : std::get<td::td_api::array<std::string>>(result)) {
                    stdvec.push_back(username);
                }
                return glz::write<glz::opts{.prettify = true}>(stdvec).value_or("ERROR");
            }
        },
        "phone_number", &T::phone_number_,
        "user_status", [] (const T& self) -> std::string {
            if (!self.status_) {
                return "ERROR";
            }

            switch (self.status_->get_id()) {
                case td::td_api::userStatusEmpty::ID:
                    return "empty";
                case td::td_api::userStatusLastMonth::ID:
                    return "last month";
                case td::td_api::userStatusLastWeek::ID:
                    return "last week";
                case td::td_api::userStatusOffline::ID: {
                    const auto* u_stat_offline = static_cast<const td::td_api::userStatusOffline*>(self.status_.get());
                    std::ostringstream os;
                    std::time_t time = static_cast<std::time_t>(u_stat_offline->was_online_);
                    os << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
                    return os.str();
                }
                case td::td_api::userStatusRecently::ID:
                    return "recently";
                case td::td_api::userStatusOnline::ID:
                    return "online";
                default:
                    return "unknown";
            }
        },
        "accent_color_id", &T::accent_color_id_,
        "background_custom_emoji_id", &T::background_custom_emoji_id_,
        "profile_accent_color_id", &T::profile_accent_color_id_,
        "profile_background_custom_emoji_id", &T::profile_background_custom_emoji_id_,
        "emoji_status", [](const T& self) -> std::string {
            if (!self.emoji_status_)
                return "ERROR";
            const auto* emoji_status_t = static_cast<const td::td_api::emojiStatus*>(self.emoji_status_.get());
            /* UPGRADED GIFTS ARE NOT SUPPORTED NOW */
            if (emoji_status_t->type_->get_id() == td::td_api::emojiStatusTypeUpgradedGift::ID) {
                spdlog::warn("{}: emojiStatusTypeUpgradedGift is not supported now");
                return glz::write_json(__impl_emojiStatus{.expiration_date = emoji_status_t->expiration_date_}).value_or("ERROR");
            }
            const auto* custom_emoji = static_cast<const td::td_api::emojiStatusTypeCustomEmoji*>(emoji_status_t->type_.get());
            return glz::write_json(__impl_emojiStatus{.expiration_date = emoji_status_t->expiration_date_, .status_emoji_id = custom_emoji->custom_emoji_id_}).value_or("ERROR");
        },
        "is_contact", &T::is_contact_,
        "is_mutal_contant", &T::is_mutual_contact_,
        "is_close_friend", &T::is_close_friend_,
        "verification_status", [] (const T& self) -> std::string {
            if (!self.verification_status_)
                return "ERROR";
            const auto* ver_status_t = static_cast<const td::td_api::verificationStatus*>(self.verification_status_.get());
            return glz::write_json(__impl_verificationStatus{.is_verified = ver_status_t->is_verified_, .is_scam = ver_status_t->is_scam_, .is_fake = ver_status_t->is_fake_, .bot_verification_icon_custom_emoji_id = ver_status_t->bot_verification_icon_custom_emoji_id_}).value_or("ERROR");
        },
        "is_premium", &T::is_premium_,
        "is_support", &T::is_support_,
        "restriction_reason", &T::restriction_reason_,
        "has_active_stories", &T::has_active_stories_,
        "restricts_new_chats", &T::restricts_new_chats_,
        "paid_message_star_count", &T::paid_message_star_count_,
        "have_access", &T::have_access_,
        "type", [] (const T& self) {
            switch (self.type_->get_id()) {
                case td::td_api::userTypeDeleted::ID:
                case td::td_api::userTypeUnknown::ID:
                    return "unknown/deleted";
                case td::td_api::userTypeBot::ID:
                    return "bot";
                case td::td_api::userTypeRegular::ID:
                    return"regular";
                default:
                    return "unknown";
            }
        },
        "language_code", &T::language_code_,
        "added_to_attachment_menu", &T::added_to_attachment_menu_
    );
};