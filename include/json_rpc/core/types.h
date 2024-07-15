#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <variant>

#include <nlohmann/json.hpp>

namespace json_rpc {

using JsonRpcMethodHandler = std::function<Response(const Request &)>;
using JsonRpcNotificationHandler = std::function<void(const Request &)>;
using JsonRpcHandler =
    std::variant<JsonRpcMethodHandler, JsonRpcNotificationHandler>;

} // namespace json_rpc

#endif // TYPES_H
