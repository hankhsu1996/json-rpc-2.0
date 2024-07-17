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
  virtual void Listen() = 0;

  // For client: send a request and get a response
  virtual Json SendMethodCall(const Json &request) = 0;

  // For client: send a notification
  virtual void SendNotification(const Json &notification) = 0;

  void SetDispatcher(Dispatcher *dispatcher) {
    dispatcher_ = dispatcher;
  }

protected:
  Dispatcher *dispatcher_;
};

} // namespace json_rpc

#endif // JSON_RPC_TRANSPORT_H
