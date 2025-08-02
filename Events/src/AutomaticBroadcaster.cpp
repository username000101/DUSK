#include "Events.hpp"

#include <thread>

void events::EventsInteractions::automatic_broadcaster(std::chrono::milliseconds sleep_time) {
    broadcast_stop = false;
    while (!broadcast_stop) {
        if (!updates.empty()) {
            auto upd_pair = std::move(updates.top());
            updates.pop();
            std::shared_ptr<td::td_api::Object> update(upd_pair.second.release());

            for (auto pair : listeners) {
                if (pair.second.first == upd_pair.first) {
                    std::thread callback([update, pair] () { pair.second.second(update); });
                    callback.detach();
                }
            }
        }
        std::this_thread::sleep_for(sleep_time);
    }
}
