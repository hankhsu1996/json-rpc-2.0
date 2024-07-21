#pragma once

#include <httplib.h>
#include <string>

#include "json_rpc/core/dispatcher.h"
#include "json_rpc/core/transport.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class HttpClientTransport : public ClientTransport {
public:
  explicit HttpClientTransport(const std::string &server_url);
  Json SendMethodCall(const Json &request) override;
  void SendNotification(const Json &notification) override;

private:
  std::string server_url_;
  httplib::Client cli_;
};

class HttpServerTransport : public ServerTransport {
public:
  HttpServerTransport(const std::string &host, int port);
  void Listen() override;
  void Stop();

private:
  std::string host_;
  int port_;
  httplib::Server svr_;
};

} // namespace json_rpc
