#pragma once

#include <httplib.h>
#include <string>

#include "json_rpc/core/client_transport.hpp"
#include "json_rpc/core/types.hpp"

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

} // namespace json_rpc
