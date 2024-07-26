#pragma once

#include <functional>
#include <optional>
#include <variant>

#include <nlohmann/json.hpp>

namespace jsonrpc {

/**
 * @brief Alias for the JSON type used in the library.
 */
using Json = nlohmann::json;

/**
 * @brief Type alias for method call handler functions.
 *
 * Method call handlers take an optional JSON object as input and return a JSON
 * object.
 */
using MethodCallHandler = std::function<Json(const std::optional<Json> &)>;

/**
 * @brief Type alias for notification handler functions.
 *
 * Notification handlers take an optional JSON object as input and do not return
 * a value.
 */
using NotificationHandler = std::function<void(const std::optional<Json> &)>;

/**
 * @brief Type alias for a handler which can be either a method call handler or
 * a notification handler.
 */
using Handler = std::variant<MethodCallHandler, NotificationHandler>;

} // namespace jsonrpc
