#include <iostream>
#include <sstream>
#include <unordered_map>

#include "json_rpc/transports/framed_stdio_transport.h"

namespace json_rpc {

void FramedTransport::SendMessage(const std::string &message) {
  std::ostringstream oss;
  oss << "Content-Length: " << message.size() << "\r\n"
      << "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      << "\r\n"
      << message;
  std::cout << oss.str() << std::flush;
}

std::string FramedTransport::ReceiveMessage() {
  std::string line;
  std::unordered_map<std::string, std::string> headers;
  int content_length = 0;

  // Read headers
  while (std::getline(std::cin, line) && !line.empty() && line != "\r") {
    std::istringstream header_stream(line);
    std::string header_key;
    if (std::getline(header_stream, header_key, ':')) {
      std::string header_value;
      if (std::getline(header_stream >> std::ws, header_value)) {
        headers[header_key] = header_value;
        if (header_key == "Content-Length") {
          content_length = std::stoi(header_value);
        }
      }
    }
  }

  if (content_length > 0) {
    // Read content
    std::string content(content_length, '\0');
    std::cin.read(&content[0], content_length);
    return content;
  }

  throw std::runtime_error("Failed to receive message");
}

} // namespace json_rpc
