#include "jsonrpc/transport/socket_transport.hpp"

#include <stdexcept>

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace transport {

SocketTransport::SocketTransport(
    const std::string &host, uint16_t port, bool isServer)
    : socket_(ioContext_), host_(host), port_(port), isServer_(isServer) {
  spdlog::info(
      "Initializing SocketTransport with host: {} and port: {}", host, port);

  if (isServer_) {
    BindAndListen();
  } else {
    Connect();
  }
}

asio::ip::tcp::socket &SocketTransport::GetSocket() {
  return socket_;
}

SocketTransport::~SocketTransport() {
  spdlog::info("Closing socket and shutting down SocketTransport.");
  std::error_code ec;
  socket_.close(ec);
  if (ec) {
    spdlog::warn("Socket close error: {}", ec.message());
  }
  ioContext_.stop();
}

void SocketTransport::Connect() {
  asio::ip::tcp::resolver resolver(ioContext_);
  auto endpoints = resolver.resolve(host_, std::to_string(port_));

  asio::steady_timer timer(ioContext_);
  timer.expires_after(std::chrono::seconds(3));

  std::error_code connectError;
  asio::async_connect(socket_, endpoints,
      [&](const asio::error_code &error, const asio::ip::tcp::endpoint &) {
        if (!error) {
          timer.cancel();
        } else {
          connectError = error;
        }
      });

  timer.async_wait([&](const asio::error_code &error) {
    if (!error) {
      connectError = asio::error::timed_out;
      socket_.close();
    }
  });

  ioContext_.run();

  if (connectError) {
    spdlog::error("Error connecting to {}:{}. Error: {}", host_, port_,
        connectError.message());
    throw std::runtime_error("Error connecting to socket");
  }
}

void SocketTransport::BindAndListen() {
  try {
    asio::ip::tcp::acceptor acceptor(
        ioContext_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_));
    acceptor.listen();
    spdlog::info("Listening on {}:{}", host_, port_);
    acceptor.accept(socket_);
    spdlog::info("Accepted connection on {}:{}", host_, port_);
  } catch (const std::exception &e) {
    spdlog::error(
        "Error binding/listening on {}:{}. Error: {}", host_, port_, e.what());
    throw std::runtime_error("Error binding/listening on socket");
  }
}

void SocketTransport::SendMessage(const std::string &message) {
  try {
    asio::write(socket_, asio::buffer(message + "\n"));
    spdlog::debug("Sent message: {}", message);
  } catch (const std::exception &e) {
    spdlog::error("Error sending message: {}", e.what());
    throw std::runtime_error("Error sending message");
  }
}

std::string SocketTransport::ReceiveMessage() {
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
