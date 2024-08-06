#pragma once

#include <asio.hpp>
#include <string>

#include "jsonrpc/transport/transport.hpp"

namespace jsonrpc {
namespace transport {

/**
 * @brief Transport implementation using Unix domain sockets.
 *
 * This class provides transport functionality over Unix domain sockets,
 * allowing communication between processes on the same machine. It supports
 * both client and server modes.
 */
class PipeTransport : public Transport {
public:
  /**
   * @brief Constructs a PipeTransport.
   * @param socketPath Path to the Unix domain socket.
   * @param isServer True if the transport acts as a server; otherwise, it acts
   * as a client.
   */
  PipeTransport(const std::string &socketPath, bool isServer);

  ~PipeTransport();

  void SendMessage(const std::string &message) override;
  std::string ReceiveMessage() override;

private:
  void RemoveExistingSocketFile();
  void Connect();
  void BindAndListen();

  asio::io_context ioContext_;
  asio::local::stream_protocol::socket socket_;
  std::string socketPath_;
  bool isServer_;
};

} // namespace transport
} // namespace jsonrpc
