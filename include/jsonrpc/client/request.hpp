#pragma once

#include <functional>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace jsonrpc::client {

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
  Request(
      std::string method, std::optional<nlohmann::json> params,
      bool is_notification, const std::function<int()> &id_generator);

  /// @brief Checks if the request requires a response.
  [[nodiscard]] auto RequiresResponse() const -> bool;

  /// @brief Returns the unique key (ID) for the request.
  [[nodiscard]] auto GetKey() const -> int;

  /// @brief Serializes the request to a JSON string.
  [[nodiscard]] auto Dump() const -> std::string;

 private:
  std::string method_;
  std::optional<nlohmann::json> params_;
  bool is_notification_;
  int id_;
};

}  // namespace jsonrpc::client
