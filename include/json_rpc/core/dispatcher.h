#pragma once
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

#include <nlohmann/json.hpp>

#include "json_rpc/core/error.h"
#include "json_rpc/core/request.h"
#include "json_rpc/core/response.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class Dispatcher {
public:
  virtual ~Dispatcher() = default;

  std::optional<nlohmann::json> dispatch(const nlohmann::json &request);

  void registerMethod(
      const std::string &method, const JsonRpcMethodHandler &handler);
  void registerNotification(
      const std::string &method, const JsonRpcNotificationHandler &handler);

private:
  std::unordered_map<std::string, JsonRpcHandler> _handlers;
};

} // namespace json_rpc

#endif // DISPATCHER_H
