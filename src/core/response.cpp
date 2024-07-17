#include "json_rpc/core/response.h"

namespace json_rpc {

// Implementation of constructor
Response::Response(const Json &response, std::optional<int> id)
    : response_(response) {
  if (id.has_value()) {
    response_["id"] = id.value();
  }
  ValidateResponse();
}

// Implementation of static methods to create error responses
Response Response::MethodNotFoundError(const std::optional<int> &id) {
  return Response(CreateErrorResponse("Method not found", -32601, id));
}

Response Response::ParseError() {
  return Response(CreateErrorResponse("Parse error", -32700, std::nullopt));
}

Response Response::InvalidRequestError() {
  return Response(CreateErrorResponse("Invalid Request", -32600, std::nullopt));
}

Response Response::InternalError(const std::optional<int> &id) {
  return Response(CreateErrorResponse("Internal error", -32603, id));
}

Response Response::InvalidParamsError(const std::optional<int> &id) {
  return Response(CreateErrorResponse("Invalid params", -32602, id));
}

// Validation implementation
void Response::ValidateResponse() {
  if (!response_.contains("result") && !response_.contains("error")) {
    throw std::invalid_argument(
        "Response must contain either 'result' or 'error' field.");
  }
  if (response_.contains("error")) {
    const auto &error = response_["error"];
    if (!error.contains("code") || !error.contains("message")) {
      throw std::invalid_argument(
          "Error object must contain 'code' and 'message' fields.");
    }
  }
}

// Serialization implementation
Json Response::ToJson() const {
  return response_;
}

// Helper function to create error responses
Json Response::CreateErrorResponse(
    const std::string &message, int code, const std::optional<int> &id) {
  Json error = {{"code", code}, {"message", message}};
  Json response = {{"error", error}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return response;
}

} // namespace json_rpc
