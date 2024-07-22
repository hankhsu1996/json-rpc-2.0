#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "json_rpc/core/request.hpp"
#include "json_rpc/core/response.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

class Dispatcher {
public:
  virtual ~Dispatcher() = default;

  // Dispatch an RPC request to the appropriate handler.
  std::optional<std::string> Dispatch(const std::string &request);

  // Register a method handler for a specific method.
  void RegisterMethodCall(
      const std::string &method, const MethodCallHandler &handler);

  // Register a notification handler for a specific method.
  void RegisterNotification(
      const std::string &method, const NotificationHandler &handler);

private:
  std::unordered_map<std::string, Handler> handlers_;

  // Helper functions
  Response HandleMethodCall(
      const Request &request, const MethodCallHandler &handler);
  void HandleNotification(
      const Request &request, const NotificationHandler &handler);
};

} // namespace json_rpc
