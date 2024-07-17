#pragma once
#ifndef JSON_RPC_TRANSPORT_H
#define JSON_RPC_TRANSPORT_H

#include <atomic>
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

  void Start() {
    running_ = true;
    Listen();
  }

  virtual void Stop() {
    running_ = false;
  }

  void SetDispatcher(Dispatcher *dispatcher) {
    dispatcher_ = dispatcher;
  }

protected:
  virtual void Listen() = 0;
  std::atomic<bool> running_{false};
  Dispatcher *dispatcher_;
};

} // namespace json_rpc

#endif // JSON_RPC_TRANSPORT_H
