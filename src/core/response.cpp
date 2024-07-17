#include "json_rpc/core/response.h"

namespace json_rpc {

// Implementation of constructor
Response::Response(const json &response, std::optional<int> id)
    : _response(response) {
  if (id.has_value()) {
    _response["id"] = id.value();
  }
  validateResponse();
}

// Implementation of static methods to create error responses
Response Response::MethodNotFoundError(const std::optional<int> &id) {
  return Response(createErrorResponse("Method not found", -32601, id));
}

Response Response::ParseError() {
  return Response(createErrorResponse("Parse error", -32700, std::nullopt));
}

Response Response::InvalidRequestError() {
  return Response(createErrorResponse("Invalid Request", -32600, std::nullopt));
}

Response Response::InternalError(const std::optional<int> &id) {
  return Response(createErrorResponse("Internal error", -32603, id));
}

Response Response::InvalidParamsError(const std::optional<int> &id) {
  return Response(createErrorResponse("Invalid params", -32602, id));
}

// Validation implementation
void Response::validateResponse() {
  if (!_response.contains("result") && !_response.contains("error")) {
    throw std::invalid_argument(
        "Response must contain either 'result' or 'error' field.");
  }
  if (_response.contains("error")) {
    const auto &error = _response["error"];
    if (!error.contains("code") || !error.contains("message")) {
      throw std::invalid_argument(
          "Error object must contain 'code' and 'message' fields.");
    }
  }
}

// Serialization implementation
json Response::to_json() const {
  return _response;
}

// Helper function to create error responses
json Response::createErrorResponse(
    const std::string &message, int code, const std::optional<int> &id) {
  json error = {{"code", code}, {"message", message}};
  json response = {{"error", error}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return response;
}

} // namespace json_rpc
