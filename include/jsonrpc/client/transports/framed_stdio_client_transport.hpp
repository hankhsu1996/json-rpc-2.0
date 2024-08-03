#pragma once

#include <string>

#include "jsonrpc/client/transports/client_transport.hpp"
#include "jsonrpc/common/transports/framed_transport.hpp"

namespace jsonrpc {
namespace client {
namespace transports {

/**
 * @brief Transport layer using framed messaging over standard I/O for JSON-RPC
 * communication.
 *
 * This class uses framed transport to send and receive JSON-RPC messages over
 * standard I/O.
 */
class FramedStdioClientTransport :
    public ClientTransport,
    protected common::transports::FramedTransport {
public:
  void SendRequest(const std::string &request) override;
  std::string ReadResponse() override;
};

} // namespace transports
} // namespace client
} // namespace jsonrpc
