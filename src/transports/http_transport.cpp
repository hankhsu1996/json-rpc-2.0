#include <spdlog/spdlog.h>

#include "json_rpc/transports/http_transport.h"

namespace json_rpc {

HttpClientTransport::HttpClientTransport(const std::string &server_url)
    : server_url_(server_url), cli_(server_url.c_str()) {
}

Json HttpClientTransport::SendMethodCall(const Json &request) {
  spdlog::debug("HttpClientTransport sending method call: {}", request.dump());

  auto res = cli_.Post("/", request.dump(), "application/json");

  if (res && res->status == 200) {
    spdlog::debug("HttpClientTransport received response: {}", res->body);
    return Json::parse(res->body);
  } else {
    throw std::runtime_error("Failed to send method call");
  }
}

void HttpClientTransport::SendNotification(const Json &notification) {
  spdlog::debug(
      "HttpClientTransport sending notification: {}", notification.dump());

  auto res = cli_.Post("/", notification.dump(), "application/json");

  if (res) {
    spdlog::debug(
        "HttpClientTransport notification sent with response code: {}",
        res->status);
  } else {
    throw std::runtime_error("Failed to send notification");
  }
}

HttpServerTransport::HttpServerTransport(const std::string &host, int port)
    : host_(host), port_(port) {
  svr_.Post("/", [this](const httplib::Request &req, httplib::Response &res) {
    if (!dispatcher_) {
      spdlog::error("Dispatcher is not set.");
      res.status = 500;
      return;
    }

    spdlog::debug("HttpServerTransport received request: {}", req.body);
    auto response = dispatcher_->Dispatch(req.body);
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

} // namespace json_rpc
