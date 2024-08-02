#include "jsonrpc/transports/framed_stdio_client_transport.hpp"

#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

#include <spdlog/spdlog.h>

namespace jsonrpc {

void FramedStdioClientTransport::SendRequest(const std::string &message) {
  spdlog::debug("FramedStdioClientTransport sending message: {}", message);
  FrameMessage(std::cout, message);
  std::cout << std::flush;
}

std::string FramedStdioClientTransport::ReadResponse() {
  std::string response = DeframeMessage(std::cin);
  spdlog::debug("FramedStdioClientTransport received message: {}", response);
  return response;
}

} // namespace jsonrpc
