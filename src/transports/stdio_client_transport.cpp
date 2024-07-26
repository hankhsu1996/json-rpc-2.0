#include "jsonrpc/transports/stdio_client_transport.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {

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

} // namespace jsonrpc
