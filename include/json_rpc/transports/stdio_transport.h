#pragma once
#ifndef STDIO_TRANSPORT_H
#define STDIO_TRANSPORT_H

#include <iostream>

#include <nlohmann/json.hpp>

#include "json_rpc/core/transport.h"

namespace json_rpc {

class StdioTransport : public Transport {
public:
  void listen() override;
  nlohmann::json sendRequest(const nlohmann::json &request) override;
  void sendNotification(const nlohmann::json &notification) override;
};

} // namespace json_rpc

#endif // STDIO_TRANSPORT_H
