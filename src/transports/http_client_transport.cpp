#include "jsonrpc/transports/http_client_transport.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {

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

} // namespace jsonrpc
