#include "Events.hpp"

#include <thread>

void events::EventsInteractions::automatic_broadcaster(std::chrono::milliseconds sleep_time) {
    broadcast_stop = false;
    while (!broadcast_stop) {
        if (!updates.empty()) {
            std::shared_ptr<td::td_api::Object> update(updates.top().second.release());
            updates.pop();

            for (auto pair : listeners) {
                if (pair.second) {
                    std::thread callback([update, pair] () { pair.second(update); });
                    callback.detach();
                }
            }
        }
        std::this_thread::sleep_for(sleep_time);
    }
}
