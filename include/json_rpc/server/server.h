#pragma once
#ifndef SERVER_H
#define SERVER_H

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
  void start();

  // Register an RPC method with the dispatcher
  void registerMethod(
      const std::string &method, const JsonRpcMethodHandler &handler);

  // Register an RPC notification with the dispatcher
  void registerNotification(
      const std::string &method, const JsonRpcNotificationHandler &handler);

private:
  std::unique_ptr<Dispatcher> _dispatcher;
  std::unique_ptr<Transport> _transport;
};

} // namespace json_rpc

#endif // SERVER_H
