#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "json_rpc/core/request.h"
#include "json_rpc/core/response.h"
#include "json_rpc/core/transport.h"

namespace json_rpc {

class Client {
public:
  Client(std::unique_ptr<Transport> transport);
  virtual ~Client();

  Response sendRequest(
      const std::string &method, const nlohmann::json &params, int id);

  void sendNotification(
      const std::string &method, const nlohmann::json &params);

private:
  std::unique_ptr<Transport> _transport;
};

} // namespace json_rpc

#endif // CLIENT_H
