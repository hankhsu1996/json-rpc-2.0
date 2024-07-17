#pragma once
#ifndef RESPONSE_H
#define RESPONSE_H

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace json_rpc {

class Response {
public:
  // Constructor that takes a JSON object and validates it
  explicit Response(
      const nlohmann::json &response, std::optional<int> id = std::nullopt);

  // Static methods to create common error responses
  static Response MethodNotFoundError(const std::optional<int> &id);
  static Response ParseError();
  static Response InvalidRequestError();
  static Response InternalError(const std::optional<int> &id);
  static Response InvalidParamsError(const std::optional<int> &id);

  // Serialize the Response object to a JSON object
  nlohmann::json to_json() const;

private:
  nlohmann::json _response;

  void validateResponse();

  static nlohmann::json createErrorResponse(
      const std::string &message, int code, const std::optional<int> &id);
};

} // namespace json_rpc

#endif // RESPONSE_H
