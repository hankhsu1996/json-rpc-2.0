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
  Client(std::unique_ptr<Transport> transport);
  ~Client() = default;

  json sendMethodCall(const std::string &method, const json &params, int id);

  void sendNotification(const std::string &method, const json &params);

private:
  std::unique_ptr<Transport> _transport;
};

} // namespace json_rpc

#endif // JSON_RPC_CLIENT_H
