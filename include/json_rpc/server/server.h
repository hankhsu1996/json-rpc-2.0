#pragma once
#ifndef JSON_RPC_SERVER_H
#define JSON_RPC_SERVER_H

#include <memory>
#include <string>

#include "json_rpc/core/dispatcher.h"
#include "json_rpc/core/transport.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class Server {
public:
  Server(std::unique_ptr<Transport> transport);

  // Start the server to begin handling requests
  void Start();

  // Register an RPC method with the dispatcher
  void RegisterMethodCall(
      const std::string &method, const MethodCallHandler &handler);

  // Register an RPC notification with the dispatcher
  void RegisterNotification(
      const std::string &method, const NotificationHandler &handler);

private:
  std::unique_ptr<Dispatcher> dispatcher_;
  std::unique_ptr<Transport> transport_;
};

} // namespace json_rpc

#endif // JSON_RPC_SERVER_H
