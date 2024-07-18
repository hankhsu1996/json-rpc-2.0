#include <iostream>
#include <optional>
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

Json FramedStdioClientTransport::SendMethodCall(const Json &request) {
  SendMessage(request.dump());
  return ReceiveMessage();
}

void FramedStdioClientTransport::SendNotification(const Json &notification) {
  SendMessage(notification.dump());
}

void FramedStdioServerTransport::Listen() {
  while (running_.load(std::memory_order_acquire)) {
    try {
      std::string content = ReceiveMessage();
      std::optional<std::string> response = dispatcher_->Dispatch(content);
      if (response.has_value()) {
        SendMessage(response.value());
      }
    } catch (const std::runtime_error &e) {
      break; // Exit loop on error
    }
  }
}

} // namespace json_rpc
