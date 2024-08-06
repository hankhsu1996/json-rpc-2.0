#include <iostream>

#include <spdlog/spdlog.h>

#include "jsonrpc/transport/stdio_transport.hpp"

namespace jsonrpc {
namespace transport {

void StdioTransport::SendMessage(const std::string &message) {
  spdlog::debug("StdioTransport sending message: {}", message);
  std::cout << message << std::endl;
}

std::string StdioTransport::ReceiveMessage() {
  std::string response;
  std::getline(std::cin, response);
  spdlog::debug("StdioTransport received response: {}", response);
  return response;
}

} // namespace transport
} // namespace jsonrpc
