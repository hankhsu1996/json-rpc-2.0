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

json StdioTransport::sendMethodCall(const json &request) {
  std::cout << request.dump() << std::endl;
  std::string line;
  std::getline(std::cin, line);
  return json::parse(line);
}

void StdioTransport::sendNotification(const json &notification) {
  std::cout << notification.dump() << std::endl;
}

} // namespace json_rpc
