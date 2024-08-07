#include "jsonrpc/transport/framed_socket_transport.hpp"

#include <stdexcept>
#include <unistd.h>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace transport {

FramedSocketTransport::FramedSocketTransport(
    const std::string &host, uint16_t port, bool isServer)
    : SocketTransport(host, port, isServer), FramedTransport() {
  spdlog::info("FramedSocketTransport initialized with host: {} and port: {}",
      host, port);
}

void FramedSocketTransport::SendMessage(const std::string &message) {
  try {
    asio::streambuf messageBuf;
    std::ostream messageStream(&messageBuf);
    FrameMessage(messageStream, message);

    asio::error_code ec;
    std::size_t bytesWritten = asio::write(GetSocket(), messageBuf.data(), ec);

    if (ec) {
      throw std::runtime_error("Error sending message: " + ec.message());
    }

    spdlog::info(
        "FramedSocketTransport sent message with {} bytes", bytesWritten);
  } catch (const std::exception &e) {
    spdlog::error("FramedSocketTransport failed to send message: {}", e.what());
    throw;
  }
}

std::string FramedSocketTransport::ReceiveMessage() {
  asio::streambuf buffer;
  asio::error_code ec;

  // Read headers until \r\n\r\n delimiter
  asio::read_until(GetSocket(), buffer, HEADER_DELIMITER, ec);
  if (ec) {
    throw std::runtime_error("Failed to read message headers: " + ec.message());
  }

  std::istream headerStream(&buffer);

  // Extract content length from the headers
  int contentLength = ReadContentLengthFromStream(headerStream);

  // Calculate how much more content we need to read
  std::size_t remainingContentLength = contentLength - buffer.size();

  // Read any remaining content directly into the buffer
  if (remainingContentLength > 0) {
    asio::read(GetSocket(), buffer.prepare(remainingContentLength), ec);
    if (ec && ec != asio::error::eof) {
      throw std::runtime_error(
          "Failed to read message content: " + ec.message());
    }
    buffer.commit(remainingContentLength);
  }

  // Convert the entire buffer to a string
  std::string content(
      asio::buffers_begin(buffer.data()), asio::buffers_end(buffer.data()));
  return content;
}

} // namespace transport
} // namespace jsonrpc
