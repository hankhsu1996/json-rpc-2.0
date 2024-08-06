#pragma once

#include <string>

namespace jsonrpc {
namespace transport {

/**
 * @brief Base class for JSON-RPC transport.
 *
 * This class defines the interface for communication transport layers.
 */
class Transport {
public:
  /// Virtual destructor.
  virtual ~Transport() = default;

  /**
   * @brief Sends a message in string to the transport layer.
   * @param request The JSON-RPC request as a string.
   */
  virtual void SendMessage(const std::string &request) = 0;

  /**
   * @brief Receives a message from the transport layer.
   * @return The JSON-RPC response as a string.
   */
  virtual std::string ReceiveMessage() = 0;
};

} // namespace transport
} // namespace jsonrpc
