#pragma once
#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <memory>

#include <nlohmann/json.hpp>

namespace json_rpc {

class Dispatcher;

class Transport {
public:
  virtual ~Transport() = default;

  // For server: start listening for incoming requests
  virtual void listen() = 0;

  // For client: send a request and get a response
  virtual nlohmann::json sendRequest(const nlohmann::json &request) = 0;

  // For client: send a notification
  virtual void sendNotification(const nlohmann::json &notification) = 0;

  void set_dispatcher(Dispatcher *dispatcher) {
    _dispatcher = dispatcher;
  }

protected:
  Dispatcher *_dispatcher;
};

} // namespace json_rpc

#endif // TRANSPORT_H
