#include <glaze/glaze.hpp>
#include <td/telegram/td_api.h>

template <>
struct glz::meta<td::td_api::updateChatLastMessage> {
	using T = td::td_api::updateChatLastMessage;
	static constexpr auto value = glz::object(
		"chat_id", &T::chat_id_,
		"last_message", [](T& self) {
			auto* last_message = static_cast<td::td_api::message*>(self.last_message_.get());
			return std::move(*last_message);
		}
	);
};