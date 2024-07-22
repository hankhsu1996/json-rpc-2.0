#pragma once

#include <httplib.h>
#include <string>

#include "json_rpc/core/dispatcher.h"
#include "json_rpc/core/server_transport.h"
#include "json_rpc/core/types.h"

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
