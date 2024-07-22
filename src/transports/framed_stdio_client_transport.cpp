#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include "json_rpc/transports/framed_stdio_client_transport.h"

namespace json_rpc {

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

} // namespace json_rpc
