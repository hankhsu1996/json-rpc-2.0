#include "jsonrpc/common/transports/framed_transport.hpp"

#include <iostream>
#include <sstream>

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace common {
namespace transports {

void FramedTransport::FrameMessage(
    std::ostream &output, const std::string &message) {
  spdlog::debug("FramedTransport framing message: {}", message);
  output << "Content-Length: " << message.size() << "\r\n"
         << "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
         << "\r\n"
         << message;
}

int FramedTransport::parseContentLength(const std::string &header_value) {
  spdlog::debug("Parsing Content-Length value: {}", header_value);
  try {
    return std::stoi(header_value);
  } catch (const std::invalid_argument &) {
    spdlog::error("Invalid Content-Length value: {}", header_value);
    throw std::runtime_error("Invalid Content-Length value");
  } catch (const std::out_of_range &) {
    spdlog::error("Content-Length value out of range: {}", header_value);
    throw std::runtime_error("Content-Length value out of range");
  }
}

std::string FramedTransport::DeframeMessage(std::istream &input) {
  spdlog::info("FramedTransport deframing message");
  std::string line;
  std::string headers;
  int content_length = 0;
  bool content_length_found = false;

  // Read headers
  while (std::getline(input, line) && !line.empty() && line != "\r") {
    headers += line + "\n";
    std::istringstream line_stream(line);
    std::string header_key;
    if (std::getline(line_stream, header_key, ':')) {
      std::string header_value;
      if (std::getline(line_stream >> std::ws, header_value)) {
        spdlog::debug("Parsed header: {}: {}", header_key, header_value);
        if (header_key == "Content-Length") {
          content_length = parseContentLength(header_value);
          content_length_found = true;
        }
      }
    }
  }

  if (!content_length_found) {
    spdlog::error("Content-Length header missing");
    throw std::runtime_error("Content-Length header missing");
  }

  // Read content
  spdlog::debug(
      "FramedTransport reading content of length: {}", content_length);
  std::string content(content_length, '\0');
  input.read(&content[0], content_length);
  if (input.gcount() != content_length) {
    throw std::runtime_error("Failed to read the expected content length");
  }
  return content;
}

} // namespace transports
} // namespace common
} // namespace jsonrpc
