#pragma once

#include <memory>
#include <string>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

class Server {
public:
  Server(std::unique_ptr<ServerTransport> transport);

  // Start the server to begin handling requests
  void Start();

  // Stop the server to stop handling requests
  void Stop();

  // Register an RPC method with the dispatcher
  void RegisterMethodCall(
      const std::string &method, const MethodCallHandler &handler);

  // Register an RPC notification with the dispatcher
  void RegisterNotification(
      const std::string &method, const NotificationHandler &handler);

  // Check if the server is running
  bool IsRunning() const {
    return transport_->IsRunning();
  }

private:
  std::unique_ptr<Dispatcher> dispatcher_;
  std::unique_ptr<ServerTransport> transport_;
};

} // namespace json_rpc
