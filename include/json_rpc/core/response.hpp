#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "json_rpc/core/types.hpp"

namespace json_rpc {

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

class Response {
public:
  // Factory method to create a Response object from a JSON object
  static Response FromJson(const Json &responseJson, std::optional<Json> id);

  // Factory method to create a successful Response object
  static Response SuccessResponse(
      const Json &result, const std::optional<Json> &id);

  // Factory method to create a Response object for a library error
  static Response CreateLibError(
      LibErrorKind errorKind, const std::optional<Json> &id = std::nullopt);

  // Factory method to create a Response object for a user error
  static Response UserErrorResponse(
      const Json &error, const std::optional<Json> &id);

  // Serialize the Response object to a JSON object
  inline Json ToJson() const {
    return response_;
  }

  // Serialize the Response object to a string
  inline std::string ToStr() const {
    return response_.dump();
  }

private:
  Json response_;

  // Private constructor to enforce factory method usage
  explicit Response(
      const Json &response, std::optional<Json> id = std::nullopt);

  void ValidateResponse() const;

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
