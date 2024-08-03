#include "jsonrpc/client/transports/stdio_client_transport.hpp"

#include <iostream>

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace client {
namespace transports {

void StdioClientTransport::SendRequest(const std::string &message) {
  spdlog::debug("StdioClientTransport sending message: {}", message);
  std::cout << message << std::endl;
}

std::string StdioClientTransport::ReadResponse() {
  std::string response;
  std::getline(std::cin, response);
  spdlog::debug("StdioClientTransport received response: {}", response);
  return response;
}

} // namespace transports
} // namespace client
} // namespace jsonrpc
