#include "jsonrpc/transport/framed_stdio_transport.hpp"

#include <iostream>

#include <spdlog/spdlog.h>

namespace jsonrpc::transport {

void FramedStdioTransport::SendMessage(const std::string &message) {
  spdlog::debug("FramedStdioTransport sending message: {}", message);
  FrameMessage(std::cout, message);
  std::cout << std::flush;
}

auto FramedStdioTransport::ReceiveMessage() -> std::string {
  std::string response = ReceiveFramedMessage(std::cin);
  spdlog::debug("FramedStdioTransport received message: {}", response);
  return response;
}

}  // namespace jsonrpc::transport
