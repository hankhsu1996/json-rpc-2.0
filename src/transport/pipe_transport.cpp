#include "jsonrpc/transport/pipe_transport.hpp"

#include <stdexcept>
#include <unistd.h>

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace transport {

PipeTransport::PipeTransport(const std::string &socketPath, bool isServer)
    : socket_(ioContext_), socketPath_(socketPath), isServer_(isServer) {
  spdlog::info("Initializing PipeTransport with socket path: {}. IsServer: {}",
      socketPath, isServer);

  if (isServer_) {
    RemoveExistingSocketFile();
    BindAndListen();
  } else {
    Connect();
  }
}

asio::local::stream_protocol::socket &PipeTransport::GetSocket() {
  return socket_;
}

PipeTransport::~PipeTransport() {
  spdlog::info("Closing socket and shutting down PipeTransport.");
  socket_.close();
  ioContext_.stop();
}

void PipeTransport::RemoveExistingSocketFile() {
  if (unlink(socketPath_.c_str()) == 0) {
    spdlog::info("Removed existing socket file: {}", socketPath_);
  } else if (errno != ENOENT) {
    spdlog::error("Failed to remove existing socket file: {}. Error: {}",
        socketPath_, strerror(errno));
    throw std::runtime_error("Failed to remove existing socket file.");
  }
}

void PipeTransport::Connect() {
  try {
    asio::local::stream_protocol::endpoint endpoint(socketPath_);
    socket_.connect(endpoint);
    spdlog::info("Connected to socket at path: {}", socketPath_);
  } catch (const std::exception &e) {
    spdlog::error("Error connecting to socket: {}", e.what());
    throw std::runtime_error("Error connecting to socket");
  }
}

void PipeTransport::BindAndListen() {
  try {
    asio::local::stream_protocol::acceptor acceptor(
        ioContext_, asio::local::stream_protocol::endpoint(socketPath_));
    acceptor.listen();
    spdlog::info("Listening on socket path: {}", socketPath_);
    acceptor.accept(socket_);
    spdlog::info("Accepted connection on socket path: {}", socketPath_);
  } catch (const std::exception &e) {
    spdlog::error("Error binding/listening on socket: {}", e.what());
    throw std::runtime_error("Error binding/listening on socket");
  }
}

void PipeTransport::SendMessage(const std::string &message) {
  try {
    std::string fullMessage = message + "\n";
    asio::write(socket_, asio::buffer(fullMessage));
    spdlog::debug("Sent message: {}", message);
  } catch (const std::exception &e) {
    spdlog::error("Error sending message: {}", e.what());
    throw std::runtime_error("Error sending message");
  }
}

std::string PipeTransport::ReceiveMessage() {
  try {
    asio::streambuf buffer;
    asio::read_until(socket_, buffer, '\n');
    std::istream is(&buffer);
    std::string message;
    std::getline(is, message);
    spdlog::debug("Received message: {}", message);
    return message;
  } catch (const std::exception &e) {
    spdlog::error("Error receiving message: {}", e.what());
    socket_.close();
    return "";
  }
}

} // namespace transport
} // namespace jsonrpc
