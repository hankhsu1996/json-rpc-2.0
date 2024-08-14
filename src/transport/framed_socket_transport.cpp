#include "jsonrpc/transport/framed_socket_transport.hpp"

#include <stdexcept>
#include <unistd.h>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace jsonrpc::transport {

FramedSocketTransport::FramedSocketTransport(
    const std::string &host, uint16_t port, bool is_server)
    : SocketTransport(host, port, is_server), FramedTransport() {
  spdlog::info(
      "FramedSocketTransport initialized with host: {} and port: {}", host,
      port);
}

void FramedSocketTransport::SendMessage(const std::string &message) {
  try {
    asio::streambuf message_buf;
    std::ostream message_stream(&message_buf);
    FrameMessage(message_stream, message);

    asio::error_code ec;
    std::size_t bytes_written =
        asio::write(GetSocket(), message_buf.data(), ec);

    if (ec) {
      throw std::runtime_error("Error sending message: " + ec.message());
    }

    spdlog::info(
        "FramedSocketTransport sent message with {} bytes", bytes_written);
  } catch (const std::exception &e) {
    spdlog::error("FramedSocketTransport failed to send message: {}", e.what());
    throw;
  }
}

auto FramedSocketTransport::ReceiveMessage() -> std::string {
  asio::streambuf buffer;
  asio::error_code ec;

  // Read headers until \r\n\r\n delimiter
  asio::read_until(GetSocket(), buffer, kHeaderDelimiter, ec);
  if (ec) {
    throw std::runtime_error("Failed to read message headers: " + ec.message());
  }

  std::istream header_stream(&buffer);

  // Extract content length from the headers
  int content_length = ReadContentLengthFromStream(header_stream);

  // Calculate how much more content we need to read
  std::size_t remaining_content_length = content_length - buffer.size();

  // Read any remaining content directly into the buffer
  if (remaining_content_length > 0) {
    asio::read(GetSocket(), buffer.prepare(remaining_content_length), ec);
    if (ec && ec != asio::error::eof) {
      throw std::runtime_error(
          "Failed to read message content: " + ec.message());
    }
    buffer.commit(remaining_content_length);
  }

  // Convert the entire buffer to a string
  std::string content(
      asio::buffers_begin(buffer.data()), asio::buffers_end(buffer.data()));
  return content;
}

}  // namespace jsonrpc::transport
