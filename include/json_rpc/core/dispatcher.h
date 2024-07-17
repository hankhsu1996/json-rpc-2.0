#pragma once
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "json_rpc/core/request.h"
#include "json_rpc/core/response.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class Dispatcher {
public:
  virtual ~Dispatcher() = default;

  // Dispatch an RPC request to the appropriate handler.
  std::optional<std::string> dispatch(const std::string &request);

  // Register a method handler for a specific method.
  void registerMethodCall(
      const std::string &method, const MethodCallHandler &handler);

  // Register a notification handler for a specific method.
  void registerNotification(
      const std::string &method, const NotificationHandler &handler);

private:
  std::unordered_map<std::string, Handler> _handlers;

  // Helper functions
  Response handleMethodCall(
      const Request &request, const MethodCallHandler &handler);
  void handleNotification(
      const Request &request, const NotificationHandler &handler);
};

} // namespace json_rpc

#endif // DISPATCHER_H
