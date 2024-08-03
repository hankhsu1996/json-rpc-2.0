#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace jsonrpc {
namespace server {

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
      const nlohmann::json &responseJson, std::optional<nlohmann::json> id);

  /**
   * @brief Creates a successful Response object.
   * @param result The result of the method call.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating success.
   */
  static Response CreateResult(
      const nlohmann::json &result, const std::optional<nlohmann::json> &id);

  /**
   * @brief Creates a Response object for a library error.
   * @param errorKind The kind of library error.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating a library error.
   */
  static Response CreateLibError(LibErrorKind errorKind,
      const std::optional<nlohmann::json> &id = std::nullopt);

  /**
   * @brief Creates a Response object for a user error.
   * @param error The JSON object representing the error.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating a user error.
   */
  static Response CreateUserError(
      const nlohmann::json &error, const std::optional<nlohmann::json> &id);

  /**
   * @brief Serializes the Response object to a JSON object.
   * @return The JSON representation of the response.
   */
  nlohmann::json ToJson() const;

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
  explicit Response(
      nlohmann::json response, std::optional<nlohmann::json> id = std::nullopt);

  /// @brief Validates the Response object.
  void ValidateResponse() const;

  /**
   * @brief Creates a JSON object representing an error response.
   * @param message The error message.
   * @param code The error code.
   * @param id The ID of the request.
   * @return The JSON representation of the error response.
   */
  static nlohmann::json CreateErrorResponse(const std::string &message,
      int code, const std::optional<nlohmann::json> &id);

  /// @brief The JSON object representing the response.
  nlohmann::json response_;

  /// @brief Static data member for mapping error kinds to messages.
  static const ErrorInfoMap errorInfoMap;
};

} // namespace server
} // namespace jsonrpc
