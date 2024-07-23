#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "json_rpc/core/types.hpp"

namespace json_rpc {

/**
 * @brief Enumeration for library error kinds.
 */
enum class LibErrorKind {
  ParseError,
  InvalidRequest,
  MethodNotFound,
  InvalidParams,
  InternalError,
  ServerError
};

using ErrorInfoMap =
    std::unordered_map<LibErrorKind, std::pair<int, const char *>>;

/**
 * @brief Represents a JSON-RPC response.
 */
class Response {
public:
  /**
   * @brief Creates a Response object from a JSON object.
   *
   * @param responseJson The JSON object representing the response.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object.
   */
  static Response FromJson(const Json &responseJson, std::optional<Json> id);

  /**
   * @brief Creates a successful Response object.
   *
   * @param result The result of the method call.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating success.
   */
  static Response SuccessResponse(
      const Json &result, const std::optional<Json> &id);

  /**
   * @brief Creates a Response object for a library error.
   *
   * @param errorKind The kind of library error.
   * @param id The ID of the request. It can be a JSON object or null. If not
   * provided, it defaults to null.
   * @return A Response object indicating a library error.
   */
  static Response CreateLibError(
      LibErrorKind errorKind, const std::optional<Json> &id = std::nullopt);

  /**
   * @brief Creates a Response object for a user error.
   *
   * @param error The JSON object representing the error.
   * @param id The ID of the request. It can be a JSON object or null.
   * @return A Response object indicating a user error.
   */
  static Response UserErrorResponse(
      const Json &error, const std::optional<Json> &id);

  /**
   * @brief Serializes the Response object to a JSON object.
   *
   * @return The JSON representation of the response.
   */
  inline Json ToJson() const {
    return response_;
  }

  /**
   * @brief Serializes the Response object to a string.
   *
   * @return The string representation of the response.
   */
  inline std::string ToStr() const {
    return response_.dump();
  }

private:
  Json response_;

  /**
   * @brief Constructs a Response object.
   *
   * @param response The JSON object representing the response.
   * @param id The ID of the request. It can be a JSON object or null.
   */
  explicit Response(
      const Json &response, std::optional<Json> id = std::nullopt);

  /// @brief Validates the Response object.
  void ValidateResponse() const;

  /// @brief Creates a JSON object representing an error response.
  static Json CreateErrorResponse(
      const std::string &message, int code, const std::optional<Json> &id);
  static const ErrorInfoMap errorInfoMap;
};

inline Response Response::CreateLibError(
    LibErrorKind errorKind, const std::optional<Json> &id) {
  const auto &[code, message] = errorInfoMap.at(errorKind);
  return Response(CreateErrorResponse(message, code, id));
}

inline Json Response::CreateErrorResponse(
    const std::string &message, int code, const std::optional<Json> &id) {
  Json error = {{"code", code}, {"message", message}};
  Json response = {{"error", error}};
  if (id.has_value()) {
    response["id"] = id.value();
  } else {
    response["id"] = nullptr;
  }
  return response;
}

} // namespace json_rpc
