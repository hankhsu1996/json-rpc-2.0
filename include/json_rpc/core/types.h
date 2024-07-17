#pragma once
#ifndef JSON_RPC_TYPES_H
#define JSON_RPC_TYPES_H

#include <functional>
#include <variant>

#include <nlohmann/json.hpp>

namespace json_rpc {

using Json = nlohmann::json;
using MethodCallHandler = std::function<Json(const Json &)>;
using NotificationHandler = std::function<void(const Json &)>;
using Handler = std::variant<MethodCallHandler, NotificationHandler>;

} // namespace json_rpc

#endif // JSON_RPC_TYPES_H
