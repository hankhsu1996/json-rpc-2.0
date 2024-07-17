#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <variant>

#include <nlohmann/json.hpp>

namespace json_rpc {

using json = nlohmann::json;
using MethodCallHandler = std::function<json(const json &)>;
using NotificationHandler = std::function<void(const json &)>;
using Handler = std::variant<MethodCallHandler, NotificationHandler>;

} // namespace json_rpc

#endif // TYPES_H
