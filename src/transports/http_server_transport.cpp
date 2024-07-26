#include "jsonrpc/transports/http_server_transport.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {

HttpServerTransport::HttpServerTransport(const std::string &host, int port)
    : host_(host), port_(port) {
  svr_.Post("/", [this](const httplib::Request &req, httplib::Response &res) {
    if (!dispatcher_) {
      spdlog::error("Dispatcher is not set.");
      res.status = 500;
      return;
    }

    spdlog::debug("HttpServerTransport received request: {}", req.body);
    auto response = dispatcher_->DispatchRequest(req.body);
    if (response.has_value()) {
      spdlog::debug(
          "HttpServerTransport sending response: {}", response.value());
      res.set_content(response.value(), "application/json");
    } else {
      res.status = 204; // No content
    }
  });
}

void HttpServerTransport::Listen() {
  spdlog::info("HttpServerTransport listening on {}:{}", host_, port_);
  svr_.listen(host_.c_str(), port_);
}

void HttpServerTransport::Stop() {
  spdlog::info("HttpServerTransport stopping");
  svr_.stop();
}

} // namespace jsonrpc
