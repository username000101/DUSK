#pragma once

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace update {
    td::ClientManager::Response send_request(td::td_api::object_ptr<td::td_api::Function> request, std::shared_ptr<td::ClientManager> client_ = nullptr, td::ClientManager::ClientId client_id_ = 0);
}