#pragma once

#include <string>

#include "jsonrpc/core/client_transport.hpp"
#include "jsonrpc/core/types.hpp"
#include "jsonrpc/transports/framed_transport.hpp"

namespace jsonrpc {

/**
 * @brief Transport layer using framed messaging over standard I/O for JSON-RPC communication.
 *
 * This class uses framed transport to send and receive JSON-RPC messages over standard I/O.
 */
class FramedStdioClientTransport :
    public ClientTransport,
    protected FramedTransport {
public:
  void SendRequest(const std::string &request) override;
  std::string ReadResponse() override;
};

} // namespace jsonrpc
