#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include "json_rpc/transports/framed_stdio_transport.h"

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

void FramedStdioServerTransport::Listen() {
  spdlog::info("FramedStdioServerTransport listening");

  if (!dispatcher_) {
    spdlog::error("Dispatcher is not set.");
    return;
  }

  while (IsRunning()) {
    try {
      std::string content = ReceiveMessage();
      std::optional<std::string> response = dispatcher_->Dispatch(content);
      if (response.has_value()) {
        spdlog::debug("FramedStdioServerTransport dispatching response: {}",
            response.value());
        SendMessage(response.value());
      }
    } catch (const std::runtime_error &e) {
      spdlog::error(
          "Runtime error in FramedStdioServerTransport: {}", e.what());
      break; // Exit loop on error
    }
  }
}

} // namespace json_rpc
