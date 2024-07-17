#pragma once
#ifndef JSON_RPC_RESPONSE_H
#define JSON_RPC_RESPONSE_H

#include <optional>
#include <string>

#include "json_rpc/core/types.h"

namespace json_rpc {

class Response {
public:
  // Constructor that takes a JSON object and validates it
  explicit Response(const json &response, std::optional<int> id = std::nullopt);

  // Static methods to create common error responses
  static Response MethodNotFoundError(const std::optional<int> &id);
  static Response ParseError();
  static Response InvalidRequestError();
  static Response InternalError(const std::optional<int> &id);
  static Response InvalidParamsError(const std::optional<int> &id);

  // Serialize the Response object to a JSON object
  json to_json() const;

private:
  json _response;

  void validateResponse();

  static json createErrorResponse(
      const std::string &message, int code, const std::optional<int> &id);
};

} // namespace json_rpc

#endif // JSON_RPC_RESPONSE_H
