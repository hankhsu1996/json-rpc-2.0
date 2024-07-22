#pragma once

#include <optional>
#include <string>

#include "json_rpc/core/types.h"

namespace json_rpc {

class Response {
public:
  // Factory methods to create responses
  static Response FromJson(const Json &responseJson, std::optional<int> id);

  static Response SuccessResponse(
      const Json &result, const std::optional<int> &id);

  static Response LibraryErrorResponse(const std::string &message, int code,
      const std::optional<int> &id = std::nullopt);

  static Response UserErrorResponse(
      const Json &error, const std::optional<int> &id);

  // Serialize the Response object to a JSON object
  Json ToJson() const;

private:
  Json response_;

  // Private constructor to enforce factory method usage
  Response(const Json &response, std::optional<int> id = std::nullopt);

  void ValidateResponse() const;

  static Json CreateErrorResponse(
      const std::string &message, int code, const std::optional<int> &id);
};

} // namespace json_rpc
