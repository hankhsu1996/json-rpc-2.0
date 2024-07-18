#pragma once

#include <optional>
#include <string>

#include "json_rpc/core/types.h"

namespace json_rpc {

class Response {
public:
  // Constructor that takes a JSON object and validates it
  explicit Response(const Json &response, std::optional<int> id = std::nullopt);

  // Static methods to create common error responses
  static Response MethodNotFoundError(const std::optional<int> &id);
  static Response ParseError();
  static Response InvalidRequestError();
  static Response InternalError(const std::optional<int> &id);
  static Response InvalidParamsError(const std::optional<int> &id);

  // Serialize the Response object to a JSON object
  Json ToJson() const;

private:
  Json response_;

  void ValidateResponse();

  static Json CreateErrorResponse(
      const std::string &message, int code, const std::optional<int> &id);
};

} // namespace json_rpc
