#pragma once
#ifndef JSON_RPC_CLIENT_H
#define JSON_RPC_CLIENT_H

#include <iostream>
#include <memory>
#include <string>

#include "json_rpc/core/request.h"
#include "json_rpc/core/response.h"
#include "json_rpc/core/transport.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class Client {
public:
  Client(std::unique_ptr<ClientTransport> transport);
  ~Client() = default;

  Json SendMethodCall(const std::string &method, const Json &params, int id);

  void SendNotification(const std::string &method, const Json &params);

private:
  std::unique_ptr<ClientTransport> transport_;
};

} // namespace json_rpc

#endif // JSON_RPC_CLIENT_H
