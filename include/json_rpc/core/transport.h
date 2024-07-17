#pragma once
#ifndef JSON_RPC_TRANSPORT_H
#define JSON_RPC_TRANSPORT_H

#include <memory>
#include <string>

#include "json_rpc/core/types.h"

namespace json_rpc {

class Dispatcher;

class ClientTransport {
public:
  virtual ~ClientTransport() = default;
  virtual Json SendMethodCall(const Json &request) = 0;
  virtual void SendNotification(const Json &notification) = 0;
};

class ServerTransport {
public:
  virtual ~ServerTransport() = default;
  virtual void Listen() = 0;

  void SetDispatcher(Dispatcher *dispatcher) {
    dispatcher_ = dispatcher;
  }

protected:
  Dispatcher *dispatcher_;
};

} // namespace json_rpc

#endif // JSON_RPC_TRANSPORT_H
