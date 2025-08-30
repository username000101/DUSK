#pragma once

#include <string>
#include <sstream>

#include <rpc/rpc_error.h>

inline std::string convert_rpc_error(rpc::rpc_error& rpcerr) {
    auto object = rpcerr.get_error().as<RPCLIB_MSGPACK::object>();

    std::ostringstream error_dump;
    error_dump << object;
    return error_dump.str();
}