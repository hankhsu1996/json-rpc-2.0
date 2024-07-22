#include <iostream>
#include <sstream>

#include <spdlog/spdlog.h>

#include "json_rpc/transports/framed_transport.h"

namespace json_rpc {

void FramedTransport::SendMessage(const std::string &message) {
  spdlog::debug("FramedTransport sending message: {}", message);
  std::ostringstream oss;
  oss << "Content-Length: " << message.size() << "\r\n"
      << "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      << "\r\n"
      << message;
  std::cout << oss.str() << std::flush;
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

void FramedTransport::readHeaders(int &content_length) {
  spdlog::info("FramedTransport reading headers");
  std::string line;
  bool content_length_found = false;
  while (std::getline(std::cin, line) && !line.empty() && line != "\r") {
    spdlog::debug("Reading header line: {}", line);
    std::istringstream header_stream(line);
    std::string header_key;
    if (std::getline(header_stream, header_key, ':')) {
      std::string header_value;
      if (std::getline(header_stream >> std::ws, header_value)) {
        spdlog::debug("Parsed header: {}: {}", header_key, header_value);
        if (header_key == "Content-Length") {
          try {
            content_length = parseContentLength(header_value);
            content_length_found = true;
          } catch (const std::runtime_error &e) {
            spdlog::error("Content-Length header parsing error: {}", e.what());
            throw; // Re-throw the parsing error
          }
        }
      }
    }
  }
  if (!content_length_found) {
    spdlog::error("Content-Length header missing");
    throw std::runtime_error("Content-Length header missing");
  }
}

std::string FramedTransport::ReceiveMessage() {
  spdlog::debug("FramedTransport receiving message");

  int content_length = 0;
  try {
    readHeaders(content_length);
  } catch (const std::runtime_error &e) {
    spdlog::error("FramedTransport header read error: {}", e.what());
    throw;
  }

  if (content_length > 0) {
    std::string content(content_length, '\0');
    std::cin.read(&content[0], content_length);
    spdlog::debug("FramedTransport message received: {}", content);
    return content;
  }

  throw std::runtime_error("Failed to receive message in FramedTransport");
}

} // namespace json_rpc
