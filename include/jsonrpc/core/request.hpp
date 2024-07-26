#pragma once

#include <optional>
#include <string>

#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

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
      const std::optional<Json> &params = std::nullopt,
      std::optional<Json> id = std::nullopt)
      : method_(method), params_(params), id_(id) {
  }

  /**
   * @brief Creates a Request object from a JSON object.
   *
   * @param jsonObj The JSON object representing the request.
   * @return A Request object.
   */
  static Request FromJson(const Json &jsonObj);

  /**
   * @brief Serializes the Request object to a JSON object.
   *
   * @return The JSON representation of the request.
   */
  Json ToJson() const;

  /// @brief Gets the method name.
  std::string GetMethod() const {
    return method_;
  }

  /// @brief Gets the parameters.
  std::optional<Json> GetParams() const {
    return params_;
  }

  /// @brief Gets the request ID.
  std::optional<Json> GetId() const {
    return id_;
  }

private:
  /// @brief The name of the method to be invoked.
  std::string method_;

  /// @brief The parameters for the method.
  std::optional<Json> params_;

  /// @brief The ID of the request.
  std::optional<Json> id_;
};

} // namespace jsonrpc
