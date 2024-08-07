#pragma once

#include <asio.hpp>
#include <string>

#include "jsonrpc/transport/transport.hpp"

namespace jsonrpc {
namespace transport {

/**
 * @brief Transport implementation using TCP/IP sockets.
 *
 * This class provides transport functionality over TCP/IP sockets,
 * supporting both client and server modes for communication over a network.
 */
class SocketTransport : public Transport {
public:
  /**
   * @brief Constructs a SocketTransport.
   * @param host The host address (IP or domain name).
   * @param port The port number.
   * @param isServer True if the transport acts as a server; false if it acts as
   * a client.
   */
  SocketTransport(const std::string &host, uint16_t port, bool isServer);

  ~SocketTransport();

  void SendMessage(const std::string &message) override;
  std::string ReceiveMessage() override;

protected:
  asio::ip::tcp::socket &GetSocket();

private:
  void Connect();
  void BindAndListen();

  asio::io_context ioContext_;
  asio::ip::tcp::socket socket_;
  std::string host_;
  uint16_t port_;
  bool isServer_;
};

} // namespace transport
} // namespace jsonrpc
