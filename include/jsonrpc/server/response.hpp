#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace jsonrpc::server {

/// @brief Enumeration for library error kinds.
enum class LibErrorKind {
  kParseError,
  kInvalidRequest,
  kMethodNotFound,
  kInternalError,
  kServerError
};

using ErrorInfoMap =
    std::unordered_map<LibErrorKind, std::pair<int, const char *>>;

/// @brief Represents a JSON-RPC response.
class Response {
 public:
  Response() = delete;
  Response(const Response &) = delete;
  auto operator=(const Response &) -> Response & = delete;

  Response(Response &&other) noexcept;
  auto operator=(Response &&other) noexcept -> Response & = delete;

  ~Response() = default;

  /**
   * @brief Creates a Response object from a JSON object that represents a user
   * response.
   * @param responseJson The JSON object representing the user response.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object.
   */
  static auto FromUserResponse(
      const nlohmann::json &response_json,
      std::optional<nlohmann::json> id) -> Response;

  /**
   * @brief Creates a successful Response object.
   * @param result The result of the method call.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating success.
   */
  static auto CreateResult(
      const nlohmann::json &result,
      const std::optional<nlohmann::json> &id) -> Response;

  /**
   * @brief Creates a Response object for a library error.
   * @param errorKind The kind of library error.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating a library error.
   */
  static auto CreateLibError(
      LibErrorKind error_kind,
      const std::optional<nlohmann::json> &id = std::nullopt) -> Response;

  /**
   * @brief Creates a Response object for a user error.
   * @param error The JSON object representing the error.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating a user error.
   */
  static auto CreateUserError(
      const nlohmann::json &error,
      const std::optional<nlohmann::json> &id) -> Response;

  /**
   * @brief Serializes the Response object to a JSON object.
   * @return The JSON representation of the response.
   */
  [[nodiscard]] auto ToJson() const -> nlohmann::json;

  /**
   * @brief Serializes the Response object to a string.
   * @return The string representation of the response.
   */
  [[nodiscard]] auto ToStr() const -> std::string;

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
  static auto CreateErrorResponse(
      const std::string &message, int code,
      const std::optional<nlohmann::json> &id) -> nlohmann::json;

  /// @brief The JSON object representing the response.
  nlohmann::json response_;

  /// @brief Static data member for mapping error kinds to messages.
  static const ErrorInfoMap kErrorInfoMap;
};

}  // namespace jsonrpc::server
