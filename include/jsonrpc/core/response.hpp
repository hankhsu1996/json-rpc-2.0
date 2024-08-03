#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

/// @brief Enumeration for library error kinds.
enum class LibErrorKind {
  ParseError,
  InvalidRequest,
  MethodNotFound,
  InternalError,
  ServerError
};

using ErrorInfoMap =
    std::unordered_map<LibErrorKind, std::pair<int, const char *>>;

/// @brief Represents a JSON-RPC response.
class Response {
public:
  /// @brief Deleted default constructor to prevent instantiation without data.
  Response() = delete;

  /// @brief Deleted copy constructor to prevent copying of Response objects.
  Response(const Response &) = delete;

  /// @brief Deleted copy assignment operator to prevent assignment.
  Response &operator=(const Response &) = delete;

  /**
   * @brief Move constructor for efficient resource management.
   * @param other The response to move from.
   */
  Response(Response &&other) noexcept;

  /**
   * @brief Creates a Response object from a JSON object that represents a user
   * response.
   * @param responseJson The JSON object representing the user response.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object.
   */
  static Response FromUserResponse(
      const Json &responseJson, std::optional<Json> id);

  /**
   * @brief Creates a successful Response object.
   * @param result The result of the method call.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating success.
   */
  static Response CreateResult(
      const Json &result, const std::optional<Json> &id);

  /**
   * @brief Creates a Response object for a library error.
   * @param errorKind The kind of library error.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating a library error.
   */
  static Response CreateLibError(
      LibErrorKind errorKind, const std::optional<Json> &id = std::nullopt);

  /**
   * @brief Creates a Response object for a user error.
   * @param error The JSON object representing the error.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating a user error.
   */
  static Response CreateUserError(
      const Json &error, const std::optional<Json> &id);

  /**
   * @brief Serializes the Response object to a JSON object.
   * @return The JSON representation of the response.
   */
  Json ToJson() const;

  /**
   * @brief Serializes the Response object to a string.
   * @return The string representation of the response.
   */
  std::string ToStr() const;

private:
  /**
   * @brief Private constructor to ensure the class is only instantiated through
   * factory methods.
   * @param response The JSON object representing the response.
   * @param id The ID of the request. It can be a JSON object or null.
   */
  explicit Response(Json response, std::optional<Json> id = std::nullopt);

  /// @brief Validates the Response object.
  void ValidateResponse() const;

  /**
   * @brief Creates a JSON object representing an error response.
   * @param message The error message.
   * @param code The error code.
   * @param id The ID of the request.
   * @return The JSON representation of the error response.
   */
  static Json CreateErrorResponse(
      const std::string &message, int code, const std::optional<Json> &id);

  /// @brief The JSON object representing the response.
  Json response_;

  /// @brief Static data member for mapping error kinds to messages.
  static const ErrorInfoMap errorInfoMap;
};

} // namespace jsonrpc
