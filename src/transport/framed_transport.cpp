#include "jsonrpc/transport/framed_transport.hpp"

#include <stdexcept>

#include "jsonrpc/utils/string_utils.hpp"

namespace jsonrpc {
namespace transport {

void FramedTransport::FrameMessage(
    std::ostream &output, const std::string &message) {
  output << "Content-Length: " << message.size() << "\r\n"
         << "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
         << "\r\n"
         << message;
}

FramedTransport::HeaderMap FramedTransport::ReadHeadersFromStream(
    std::istream &input) {
  HeaderMap headers;
  std::string line;

  while (std::getline(input, line) && !line.empty() && line != "\r") {
    auto colonPos = line.find(':');
    if (colonPos != std::string::npos) {
      std::string header_key = utils::trim(line.substr(0, colonPos));
      std::string header_value = utils::trim(line.substr(colonPos + 1));
      headers[header_key] = header_value;
    }
  }

  if (headers.empty()) {
    throw std::runtime_error("Failed to read headers");
  }

  return headers;
}

int FramedTransport::ReadContentLengthFromStream(std::istream &input) {
  auto headers = ReadHeadersFromStream(input);
  auto it = headers.find("Content-Length");
  if (it == headers.end()) {
    throw std::runtime_error("Content-Length header missing");
  }
  return ParseContentLength(it->second);
}

std::string FramedTransport::ReadContent(
    std::istream &input, int content_length) {
  std::string content(content_length, '\0');
  input.read(&content[0], content_length);
  if (input.gcount() != content_length) {
    throw std::runtime_error("Failed to read the expected content length");
  }
  return content;
}

std::string FramedTransport::ReceiveFramedMessage(std::istream &input) {
  int content_length = ReadContentLengthFromStream(input);
  return ReadContent(input, content_length);
}

int FramedTransport::ParseContentLength(const std::string &header_value) {
  try {
    return std::stoi(header_value);
  } catch (const std::invalid_argument &) {
    throw std::runtime_error("Invalid Content-Length value");
  } catch (const std::out_of_range &) {
    throw std::runtime_error("Content-Length value out of range");
  }
}

} // namespace transport
} // namespace jsonrpc
