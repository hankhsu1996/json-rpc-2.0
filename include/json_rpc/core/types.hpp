#pragma once

#include <functional>
#include <optional>
#include <variant>

#include <nlohmann/json.hpp>

namespace json_rpc {

using Json = nlohmann::json;
using MethodCallHandler = std::function<Json(const std::optional<Json> &)>;
using NotificationHandler = std::function<void(const std::optional<Json> &)>;
using Handler = std::variant<MethodCallHandler, NotificationHandler>;

} // namespace json_rpc
