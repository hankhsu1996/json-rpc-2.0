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
  socket_.close();
  ioContext_.stop();
}

void SocketTransport::Connect() {
  try {
    asio::ip::tcp::resolver resolver(ioContext_);
    asio::ip::tcp::resolver::results_type endpoints =
        resolver.resolve(host_, std::to_string(port_));
    asio::steady_timer timer(ioContext_);
    bool connected = false;

    timer.expires_after(std::chrono::seconds(3)); // Set timeout duration
    timer.async_wait([&](const asio::error_code &error) {
      if (!error) {
        socket_.close(); // Close the socket if the timer expires
        spdlog::error("Connection attempt timed out after 3 seconds.");
      }
    });

    asio::async_connect(socket_, endpoints,
        [&](const asio::error_code &error, const asio::ip::tcp::endpoint &) {
          timer.cancel(); // Cancel the timer if connection succeeds
          if (!error) {
            connected = true;
          }
        });

    ioContext_.run();

    if (!connected) {
      throw std::runtime_error(
          "Failed to connect within the specified timeout.");
    }
  } catch (const std::exception &e) {
    spdlog::error(
        "Error connecting to {}:{}. Error: {}", host_, port_, e.what());
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
    std::string fullMessage = message + "\n";
    asio::write(socket_, asio::buffer(fullMessage));
    spdlog::debug("Sent message: {}", message);
  } catch (const std::exception &e) {
    spdlog::error("Error sending message: {}", std::string(e.what()));
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
