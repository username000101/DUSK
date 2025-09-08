#include "Auth.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "ConfirmDialog.hpp"
#include "Input.hpp"
#include "Updates.hpp"

td::td_api::object_ptr<td::td_api::Object> get_authorization_state() {
    return update::send_request(td::td_api::make_object<td::td_api::getAuthorizationState>()).object;
}

bool auth::authorize() {
    static std::shared_ptr<spdlog::logger> logger = nullptr;
    if (!logger) {
        logger = std::make_shared<spdlog::logger>("Auth::authorize", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
        spdlog::initialize_logger(logger);
    }

    bool have_password = false;
    std::string phone_number, code, password, password_hint;

    for (unsigned int attempt = 1; attempt <= 4000; ++attempt) {
        auto current_authorization_state = update::send_request(td::td_api::make_object<td::td_api::getAuthorizationState>());
        if (!current_authorization_state.object) {
            logger->warn("Returned invalid response; trying again({} attempts left)",
                         5 - attempt);
            continue;
        } else if (attempt == 3999) {
            logger->critical("Attempts ended, aborting");
            std::exit(-1);
        } else {
            switch (current_authorization_state.object->get_id()) {
                case td::td_api::authorizationStateReady::ID: {
                    std::string account_details;
                    auto me_info_res = update::send_request(td::td_api::make_object<td::td_api::getMe>());
                    if (me_info_res.object) {
                        auto me_info_obj = td::move_tl_object_as<td::td_api::user>(me_info_res.object);
                        account_details = me_info_obj->first_name_ + "/" + *me_info_obj->usernames_->active_usernames_.begin();
                    }
                    logger->info("Already authorized {}",
                                 (account_details.empty() ? "" : "as " + account_details));
                    return true;
                }
            }
            break;
        }
    }

    while (true) {
        phone_number = input("Phone number: ");
        if (phone_number.empty()) {
            std::cout << "Incorrect input, please try again" << std::endl;
            continue;
        }
        if (confirm("\"" + phone_number + "\" - correct?")) break;
    }

    for (unsigned int attempt = 1; attempt <= 4000; ++attempt) {
        auto phone_number_result = update::send_request(td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(phone_number, nullptr));
        if (!phone_number_result.object) {
            logger->debug("Returned invalid response; trying again({} attempts left)",
                         4000 - attempt);
            continue;
        } else if (attempt == 3999) {
            logger->critical("Attempts ended, aborting");
            std::exit(-1);
        } else {
            if (phone_number_result.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(phone_number_result.object);
                logger->warn("Returned error: '{}', trying again({} attempts left)",
                             error->message_, 5 - attempt);
            } else
                break;
        }
    }

    while (true) {
        code = input("Code: ");
        if (code.empty()) {
            std::cout << "Incorrect input, please try again" << std::endl;
            continue;
        }
        if (confirm("\"" + code + "\" - correct?"))
            break;

    }

    for (unsigned int attempt = 1; attempt <= 4000; ++attempt) {
        auto code_result = update::send_request(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code));
        if (!code_result.object) {
            logger->debug("Returned invalid response; trying again({} attempts left)",
                         4000 - attempt);
            continue;
        } else if (attempt == 3999) {
            logger->critical("Attempts ended, aborting");
            std::exit(-1);
        } else {
            if (code_result.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(code_result.object);
                logger->warn("Returned error: '{}', trying again({} attempts left)",
                             error->message_, 5 - attempt);
            } else
                break;
        }
    }

    for (unsigned int attempt = 1; attempt <= 4000; ++attempt) {
        auto current_authorization_state = update::send_request(td::td_api::make_object<td::td_api::getAuthorizationState>());
        if (!current_authorization_state.object) {
            logger->debug("Returned invalid response; trying again({} attempts left)",
                         4000 - attempt);
            continue;
        } else if (attempt == 3999) {
            logger->critical("Attempts ended, aborting");
            std::exit(-1);
        } else {
            switch (current_authorization_state.object->get_id()) {
                case td::td_api::authorizationStateWaitPassword::ID: {
                    password_hint = td::move_tl_object_as<td::td_api::authorizationStateWaitPassword>(current_authorization_state.object)->password_hint_;
                    have_password = true;
                    break;
                }
                case td::td_api::authorizationStateReady::ID:
                    return true;
            }
            break;
        }
    }

    while (true) {
        password = input("Password(" + password_hint + "): ");
        if (password.empty()) {
            std::cout << "Incorrect input, please try again" << std::endl;
            continue;
        }
        if (confirm("\"" + password + "\" - correct?"))
            break;

    }

    for (int attempt = 1; attempt <= 4000; ++attempt) {
        auto password_result = update::send_request(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password));
        if (!password_result.object) {
            logger->debug("Returned invalid response; trying again({} attempts left)",
                4000 - attempt);
            continue;
        }
        else if (attempt == 3999) {
            logger->critical("Attempts ended, aborting");
            std::exit(-1);
        }
        else {
            if (password_result.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(password_result.object);
                logger->warn("Returned error: '{}', trying again({} attempts left)",
                    error->message_, 5 - attempt);
            } else
                break;
        }
    }

    for (int attempt = 1; attempt <= 4000; ++attempt) {
        auto current_authorization_state = update::send_request(td::td_api::make_object<td::td_api::getAuthorizationState>());
        if (!current_authorization_state.object) {
            logger->debug("Returned invalid response; trying again({} attempts left)",
                4000 - attempt);
            continue;
        }
        else if (attempt == 3999) {
            logger->critical("Attempts ended, aborting");
            std::exit(-1);
        }
        else {
            switch (current_authorization_state.object->get_id()) {
                case td::td_api::authorizationStateReady::ID:
                    return true;
                default:
                    logger->error("Unexpected case: {}; trying again({} attempts left)",
                        current_authorization_state.object->get_id(), 5 - attempt);
                    break;
            }
        }
    }
    return true;
}
