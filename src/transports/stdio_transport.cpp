#include "json_rpc/core/dispatcher.h"
#include "json_rpc/transports/stdio_transport.h"

namespace json_rpc {

void StdioTransport::listen() {
  std::string line;
  while (std::getline(std::cin, line)) {
    std::optional<std::string> response = _dispatcher->dispatch(line);
    if (response.has_value()) {
      std::cout << response.value() << std::endl;
    }
  }
}

nlohmann::json StdioTransport::sendMethodCall(const nlohmann::json &request) {
  std::cout << request.dump() << std::endl;
  std::string line;
  std::getline(std::cin, line);
  return nlohmann::json::parse(line);
}

void StdioTransport::sendNotification(const nlohmann::json &notification) {
  std::cout << notification.dump() << std::endl;
}

} // namespace json_rpc
