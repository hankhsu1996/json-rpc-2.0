#include "json_rpc/core/dispatcher.h"
#include "json_rpc/transports/stdio_transport.h"

namespace json_rpc {

void StdioTransport::listen() {
  std::string line;
  while (std::getline(std::cin, line)) {
    nlohmann::json request = nlohmann::json::parse(line);
    std::optional<nlohmann::json> response = _dispatcher->dispatch(request);
    if (response.has_value()) {
      std::cout << response->dump() << std::endl;
    }
  }
}

nlohmann::json StdioTransport::sendRequest(const nlohmann::json &request) {
  std::cout << request.dump() << std::endl;
  std::string line;
  std::getline(std::cin, line);
  return nlohmann::json::parse(line);
}

void StdioTransport::sendNotification(const nlohmann::json &notification) {
  std::cout << notification.dump() << std::endl;
}

} // namespace json_rpc
