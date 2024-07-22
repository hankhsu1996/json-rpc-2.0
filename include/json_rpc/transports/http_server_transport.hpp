#pragma once

#include <httplib.h>
#include <string>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

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
