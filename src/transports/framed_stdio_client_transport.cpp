#include "jsonrpc/transports/framed_stdio_client_transport.hpp"

#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

#include <spdlog/spdlog.h>

namespace jsonrpc {

Json FramedStdioClientTransport::SendMethodCall(const Json &request) {
  spdlog::debug(
      "FramedStdioClientTransport sending method call: {}", request.dump());
  SendMessage(request.dump());
  return ReceiveMessage();
}

void FramedStdioClientTransport::SendNotification(const Json &notification) {
  spdlog::debug("FramedStdioClientTransport sending notification: {}",
      notification.dump());
  SendMessage(notification.dump());
}

} // namespace jsonrpc
