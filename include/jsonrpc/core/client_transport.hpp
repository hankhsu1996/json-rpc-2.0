#pragma once

#include <string>

#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

/**
 * @brief Base class for JSON-RPC client transport.
 *
 * This class defines the interface for communication transport layers.
 */
class ClientTransport {
public:
  /// Virtual destructor.
  virtual ~ClientTransport() = default;

  /**
   * @brief Sends a JSON-RPC request.
   * @param request The JSON-RPC request as a string.
   */
  virtual void SendRequest(const std::string &request) = 0;

  /**
   * @brief Reads a JSON-RPC response.
   * @return The JSON-RPC response as a string.
   */
  virtual std::string ReadResponse() = 0;
};

} // namespace jsonrpc
