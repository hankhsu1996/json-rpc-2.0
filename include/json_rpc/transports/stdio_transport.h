#pragma once
#ifndef JSON_RPC_STDIO_TRANSPORT_H
#define JSON_RPC_STDIO_TRANSPORT_H

#include <iostream>

#include "json_rpc/core/dispatcher.h"
#include "json_rpc/core/transport.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class StdioTransport : public Transport {
public:
  void listen() override;
  json sendMethodCall(const json &request) override;
  void sendNotification(const json &notification) override;
};

} // namespace json_rpc

#endif // JSON_RPC_STDIO_TRANSPORT_H
