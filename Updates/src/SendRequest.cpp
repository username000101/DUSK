#include "Updates.hpp"

#include <algorithm>
#include <cstdint>

#include <unordered_map>
#include <vector>

std::unordered_map<td::ClientManager::RequestId, td::ClientManager::Response> unconfirmed_updates;

std::vector<std::int32_t> ignore_update_ids = {
    td::td_api::updateAuthorizationState::ID,
    td::td_api::updateOption::ID,
    td::td_api::updateDefaultBackground::ID,
    td::td_api::updateFileDownloads::ID,
    td::td_api::updateConnectionState::ID,
};

td::ClientManager::Response update::send_request(td::td_api::object_ptr<td::td_api::Function> request, std::shared_ptr<td::ClientManager> client_, td::ClientManager::ClientId client_id_) {
    if (!request)
        return {};

    static auto logger = std::make_shared<spdlog::logger>("Updates::send_request", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
    static decltype(client_) client;
    if (!client) {
        logger->debug("Looks like this function running for the first time; initializing the client variable with value (addr){}",
                      reinterpret_cast<std::uintptr_t>(client_.get()));
        client = client_;
    }
    static std::int64_t request_id = 0;
    static td::ClientManager::ClientId client_id = 0;
    if (client_id == 0) {
        logger->debug("Looks like this function running for the first time; initializing the client_id variable with value (td::ClientManager::ClientId){}",
          client_id_);
        client_id = client_id_;
    }

    auto current_req_id = ++request_id;

    if (unconfirmed_updates.contains(current_req_id)) {
        logger->debug("Update for request id: {} already received(in unconfirmed_updates)",
                      current_req_id);
        auto result = std::move(unconfirmed_updates.at(current_req_id));
        unconfirmed_updates.erase(current_req_id);
        return result;
    }

    client->send(client_id, current_req_id, std::move(request));

    unsigned short attempts = 15;
    double timeout = 15.0;
    for (auto i = attempts; i > 0; --i) {
        logger->trace("Trying to get response for request id: {}(attempt until fail: {})",
                      current_req_id, attempts);

        auto update = client->receive(timeout);
        if (update.client_id == 0 || update.request_id == 0) {
            if (update.object && std::ranges::find(ignore_update_ids, update.object->get_id()) != ignore_update_ids.end()) {
                --i;
                continue;
            }
            logger->warn("Received an incorrect update(update::object, update::client_id or update::request_id is invalid)(update id: {})",
                          (update.object ? std::to_string(update.object->get_id()) : "NULLPTR"));
            continue;
        }

        if (update.request_id != static_cast<decltype(update.request_id)>(current_req_id)) {
            logger->debug("It looks like i'm receive response for other update(current request_id is: {}, received update has request_id: {})",
                          current_req_id, update.request_id);
            unconfirmed_updates[current_req_id] = std::move(update);
            continue;
        }

        return update;
    }

    logger->warn("Attempt are ended but response for request id: {} doesn't received...",
                 current_req_id);
    return {};
}
