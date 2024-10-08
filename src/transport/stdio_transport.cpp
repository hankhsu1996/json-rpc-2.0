#include "jsonrpc/transport/stdio_transport.hpp"

#include <iostream>

#include <spdlog/spdlog.h>

namespace jsonrpc::transport {

void StdioTransport::SendMessage(const std::string &message) {
  spdlog::debug("StdioTransport sending message: {}", message);
  std::cout << message << std::endl;
}

auto StdioTransport::ReceiveMessage() -> std::string {
  std::string response;
  if (!std::getline(std::cin, response)) {
    throw std::runtime_error("Failed to receive message");
  }
  spdlog::debug("StdioTransport received response: {}", response);
  return response;
}

}  // namespace jsonrpc::transport
