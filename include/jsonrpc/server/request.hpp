#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace jsonrpc {
namespace server {

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
  explicit Request(const std::string &method,
      const std::optional<nlohmann::json> &params = std::nullopt,
      std::optional<nlohmann::json> id = std::nullopt)
      : method_(method), params_(params), id_(id) {
  }

  /**
   * @brief Creates a Request object from a JSON object.
   *
   * @param jsonObj The JSON object representing the request.
   * @return A Request object.
   */
  static Request FromJson(const nlohmann::json &jsonObj);

  /**
   * @brief Serializes the Request object to a JSON object.
   *
   * @return The JSON representation of the request.
   */
  nlohmann::json ToJson() const;

  /// @brief Gets the method name.
  std::string GetMethod() const {
    return method_;
  }

  /// @brief Gets the parameters.
  std::optional<nlohmann::json> GetParams() const {
    return params_;
  }

  /// @brief Gets the request ID.
  std::optional<nlohmann::json> GetId() const {
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

} // namespace server
} // namespace jsonrpc
