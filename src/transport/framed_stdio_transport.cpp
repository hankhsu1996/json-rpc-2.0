#include "jsonrpc/transport/framed_stdio_transport.hpp"

#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace transport {

void FramedStdioTransport::SendMessage(const std::string &message) {
  spdlog::debug("FramedStdioTransport sending message: {}", message);
  FrameMessage(std::cout, message);
  std::cout << std::flush;
}

std::string FramedStdioTransport::ReceiveMessage() {
  std::string response = ReceiveFramedMessage(std::cin);
  spdlog::debug("FramedStdioTransport received message: {}", response);
  return response;
}

} // namespace transport
} // namespace jsonrpc
