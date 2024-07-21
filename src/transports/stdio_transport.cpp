#include <spdlog/spdlog.h>

#include "json_rpc/transports/stdio_transport.h"

namespace json_rpc {

void StdioServerTransport::Listen() {
  spdlog::info("StdioServerTransport listening");
  std::string line;
  while (running_.load(std::memory_order_acquire)) {
    if (std::getline(std::cin, line)) {
      std::optional<std::string> response = dispatcher_->Dispatch(line);
      if (response.has_value()) {
        spdlog::debug(
            "StdioServerTransport dispatching response: {}", response.value());
        std::cout << response.value() << std::endl;
      }
    } else {
      break; // Exit loop if std::getline fails (e.g., EOF)
    }
  }
}

Json StdioClientTransport::SendMethodCall(const Json &request) {
  spdlog::debug("StdioClientTransport sending method call: {}", request.dump());
  std::cout << request.dump() << std::endl;
  std::string line;
  std::getline(std::cin, line);
  spdlog::debug("StdioClientTransport received response: {}", line);
  return Json::parse(line);
}

void StdioClientTransport::SendNotification(const Json &notification) {
  spdlog::debug(
      "StdioClientTransport sending notification: {}", notification.dump());
  std::cout << notification.dump() << std::endl;
}

} // namespace json_rpc
