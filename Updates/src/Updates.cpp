#include "Updates.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <deque>
#include <thread>
#include <unordered_map>

#include <fair_mutex.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Events.hpp"

std::unordered_map<td::ClientManager::RequestId, td::ClientManager::Response> unconfirmed_updates;

std::deque<td::td_api::object_ptr<td::td_api::Object>> updates;
yamc::fair::mutex updates_mtx;

void update::updates_broadcaster() {
    static auto logger = std::make_shared<spdlog::logger>("Updates::updates_broadcaster", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
    std::thread broadcaster_t([]() { events::EventsInteractions::automatic_broadcaster(std::chrono::milliseconds(100)); });
    broadcaster_t.detach();
    logger->info("Updates and Events broadcasters has been runing");

    while (true) {
        if (!updates.empty()) {
            std::lock_guard<yamc::fair::mutex> lock(updates_mtx);
            auto update = std::move(updates.back());
            events::EventsInteractions::append_update(update);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

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

    constexpr unsigned short attempts = 15;
    constexpr double timeout = 15.0;
    for (auto i = attempts; i > 0; --i) {
        logger->trace("Trying to get response for request id: {}(attempt until fail: {})",
                      current_req_id, attempts - 1);

        auto update = client->receive(timeout);
        if (update.request_id == 0 && update.client_id != 0 && update.object) {
            updates_mtx.lock();
            updates.push_back(std::move(update.object));
            updates_mtx.unlock();
            --i;
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
