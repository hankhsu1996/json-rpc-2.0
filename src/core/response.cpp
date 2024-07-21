#include <spdlog/spdlog.h>

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
  Response response =
      Response(CreateErrorResponse("Method not found", -32601, id));
  return response;
}

Response Response::ParseError() {
  Response response =
      Response(CreateErrorResponse("Parse error", -32700, std::nullopt));
  return response;
}

Response Response::InvalidRequestError() {
  Response response =
      Response(CreateErrorResponse("Invalid Request", -32600, std::nullopt));
  return response;
}

Response Response::InternalError(const std::optional<int> &id) {
  Response response =
      Response(CreateErrorResponse("Internal error", -32603, id));
  return response;
}

Response Response::InvalidParamsError(const std::optional<int> &id) {
  Response response =
      Response(CreateErrorResponse("Invalid params", -32602, id));
  return response;
}

// Validation implementation
void Response::ValidateResponse() {
  if (!response_.contains("result") && !response_.contains("error")) {
    spdlog::error("Response validation failed: missing 'result' or 'error'");
    throw std::invalid_argument(
        "Response must contain either 'result' or 'error' field.");
  }
  if (response_.contains("error")) {
    const auto &error = response_["error"];
    if (!error.contains("code") || !error.contains("message")) {
      spdlog::error("Response validation failed: missing 'code' or 'message' "
                    "in error object");
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
