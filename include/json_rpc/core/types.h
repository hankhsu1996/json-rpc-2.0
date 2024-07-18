#pragma once

#include <functional>
#include <variant>

#include <nlohmann/json.hpp>

namespace json_rpc {

using Json = nlohmann::json;
using MethodCallHandler = std::function<Json(const Json &)>;
using NotificationHandler = std::function<void(const Json &)>;
using Handler = std::variant<MethodCallHandler, NotificationHandler>;

} // namespace json_rpc
