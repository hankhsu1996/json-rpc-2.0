#pragma once
#ifndef JSON_RPC_TRANSPORT_H
#define JSON_RPC_TRANSPORT_H

#include <memory>
#include <string>

#include "json_rpc/core/types.h"

namespace json_rpc {

class Dispatcher;

class Transport {
public:
  virtual ~Transport() = default;

  // For server: start listening for incoming requests
  virtual void listen() = 0;

  // For client: send a request and get a response
  virtual json sendMethodCall(const json &request) = 0;

  // For client: send a notification
  virtual void sendNotification(const json &notification) = 0;

  void set_dispatcher(Dispatcher *dispatcher) {
    _dispatcher = dispatcher;
  }

protected:
  Dispatcher *_dispatcher;
};

} // namespace json_rpc

#endif // JSON_RPC_TRANSPORT_H
