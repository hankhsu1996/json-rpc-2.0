#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

#include "json_rpc/transports/framed_stdio_transport.h"

namespace json_rpc {

Json FramedStdioClientTransport::SendMethodCall(const Json &request) {
  SendMessage(request.dump());
  return ReceiveMessage();
}

void FramedStdioClientTransport::SendNotification(const Json &notification) {
  SendMessage(notification.dump());
}

void FramedStdioServerTransport::Listen() {
  while (running_.load(std::memory_order_acquire)) {
    try {
      std::string content = ReceiveMessage();
      std::optional<std::string> response = dispatcher_->Dispatch(content);
      if (response.has_value()) {
        SendMessage(response.value());
      }
    } catch (const std::runtime_error &e) {
      break; // Exit loop on error
    }
  }
}

} // namespace json_rpc
