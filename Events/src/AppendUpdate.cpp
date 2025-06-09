#include "Events.hpp"

void events::EventsInteractions::append_update(td::td_api::object_ptr<td::td_api::Update> update) {
    updates.emplace(EventType::UPDATE_NEW_MESSAGE, std::move(update));
}