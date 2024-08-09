#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace jsonrpc::server {

/**
 * @brief Represents a JSON-RPC request.
 */
class Request {
 public:
  /**
   * @brief Constructs a Request object.
   *
   * @param method The name of the method to be invoked.
   * @param params The parameters for the method (optional).
   * @param id The ID of the request (optional).
   */
  explicit Request(
      std::string method,
      const std::optional<nlohmann::json> &params = std::nullopt,
      std::optional<nlohmann::json> id = std::nullopt);

  /**
   * @brief Creates a Request object from a JSON object.
   *
   * @param jsonObj The JSON object representing the request.
   * @return A Request object.
   */
  static auto FromJson(const nlohmann::json &json_obj) -> Request;

  /**
   * @brief Serializes the Request object to a JSON object.
   *
   * @return The JSON representation of the request.
   */
  [[nodiscard]] auto ToJson() const -> nlohmann::json;

  /// @brief Gets the method name.
  [[nodiscard]] auto GetMethod() const -> std::string {
    return method_;
  }

  /// @brief Gets the parameters.
  [[nodiscard]] auto GetParams() const -> std::optional<nlohmann::json> {
    return params_;
  }

  /// @brief Gets the request ID.
  [[nodiscard]] std::optional<nlohmann::json> GetId() const {
    return id_;
  }

 private:
  /// @brief The name of the method to be invoked.
  std::string method_;

  /// @brief The parameters for the method.
  std::optional<nlohmann::json> params_;

  /// @brief The ID of the request.
  std::optional<nlohmann::json> id_;
};

}  // namespace jsonrpc::server
