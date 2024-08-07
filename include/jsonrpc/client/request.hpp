#pragma once

#include <functional>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace jsonrpc {
namespace client {

/**
 * @brief Represents a JSON-RPC request.
 *
 * This class handles the creation and management of JSON-RPC requests,
 * including notifications (requests that do not expect a response).
 */
class Request {
public:
  /**
   * @brief Constructs a new Request object.
   *
   * @param method The name of the method to be invoked.
   * @param params Optional parameters to be passed with the request.
   * @param isNotification True if this is a notification (no response
   * expected).
   * @param idGenerator A function to generate unique request IDs.
   */
  Request(const std::string &method, std::optional<nlohmann::json> params,
      bool isNotification, const std::function<int()> &idGenerator);

  /// @brief Checks if the request requires a response.
  bool RequiresResponse() const;

  /// @brief Returns the unique key (ID) for the request.
  int GetKey() const;

  /// @brief Serializes the request to a JSON string.
  std::string Dump() const;

private:
  std::string method_;
  std::optional<nlohmann::json> params_;
  bool isNotification_;
  int id_;
};

} // namespace client
} // namespace jsonrpc
