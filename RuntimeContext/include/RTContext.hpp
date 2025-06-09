#pragma once

#include <cstdint>

#include <td/telegram/Client.h>

namespace rtcontext {
    extern td::ClientManager::ClientId global_client_id;
    extern td::ClientManager::RequestId global_request_id;
}