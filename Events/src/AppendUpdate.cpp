#include "Events.hpp"

void events::EventsInteractions::append_update(td::td_api::object_ptr<td::td_api::Object>&& update) {
    if (!update)
        return;
    switch (update->get_id()) {
        case td::td_api::updateNewMessage::ID:
            updates.emplace(td::td_api::updateNewMessage::ID, std::move(update));
            break;
        case td::td_api::updateMessageEdited::ID:
            updates.emplace(td::td_api::updateMessageEdited::ID, std::move(update));
            break;
        default:
            spdlog::debug("Unknown EventType: {}",
                         update->get_id());
            break;
    }
}