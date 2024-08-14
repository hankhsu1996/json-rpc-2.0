#include "jsonrpc/transport/socket_transport.hpp"

#include <stdexcept>

#include <spdlog/spdlog.h>

namespace jsonrpc::transport {

SocketTransport::SocketTransport(
    const std::string &host, uint16_t port, bool is_server)
    : socket_(io_context_), host_(host), port_(port), is_server_(is_server) {
  spdlog::info(
      "Initializing SocketTransport with host: {} and port: {}", host, port);

  if (is_server_) {
    BindAndListen();
  } else {
    Connect();
  }
}

auto SocketTransport::GetSocket() -> asio::ip::tcp::socket & {
  return socket_;
}

SocketTransport::~SocketTransport() {
  spdlog::info("Closing socket and shutting down SocketTransport.");
  std::error_code ec;
  socket_.close(ec);
  if (ec) {
    spdlog::warn("Socket close error: {}", ec.message());
  }
  io_context_.stop();
}

void SocketTransport::Connect() {
  asio::ip::tcp::resolver resolver(io_context_);
  auto endpoints = resolver.resolve(host_, std::to_string(port_));

  asio::steady_timer timer(io_context_);
  timer.expires_after(std::chrono::seconds(3));

  std::error_code connect_error;
  asio::async_connect(
      socket_, endpoints,
      [&](const asio::error_code &error, const asio::ip::tcp::endpoint &) {
        if (!error) {
          timer.cancel();
        } else {
          connect_error = error;
        }
      });

  timer.async_wait([&](const asio::error_code &error) {
    if (!error) {
      connect_error = asio::error::timed_out;
      socket_.close();
    }
  });

  io_context_.run();

  if (connect_error) {
    spdlog::error(
        "Error connecting to {}:{}. Error: {}", host_, port_,
        connect_error.message());
    throw std::runtime_error("Error connecting to socket");
  }
}

void SocketTransport::BindAndListen() {
  try {
    asio::ip::tcp::acceptor acceptor(
        io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_));
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

auto SocketTransport::ReceiveMessage() -> std::string {
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

}  // namespace jsonrpc::transport
