#include "jsonrpc/core/response.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {

const ErrorInfoMap Response::errorInfoMap = {
    {LibErrorKind::ParseError, {-32700, "Parse error"}},
    {LibErrorKind::InvalidRequest, {-32600, "Invalid Request"}},
    {LibErrorKind::MethodNotFound, {-32601, "Method not found"}},
    {LibErrorKind::InvalidParams, {-32602, "Invalid params"}},
    {LibErrorKind::InternalError, {-32603, "Internal error"}},
    {LibErrorKind::ServerError, {-32000, "Server error"}}};

Response::Response(const Json &response, std::optional<Json> id)
    : response_(response) {
  if (id.has_value()) {
    response_["id"] = id.value();
  }
  ValidateResponse();
}

Response Response::FromJson(const Json &responseJson, std::optional<Json> id) {
  if (responseJson.contains("result")) {
    return SuccessResponse(responseJson["result"], id);
  } else if (responseJson.contains("error")) {
    return UserErrorResponse(responseJson["error"], id);
  } else {
    throw std::invalid_argument(
        "Response JSON must contain either 'result' or 'error' field");
  }
}

Response Response::SuccessResponse(
    const Json &result, const std::optional<Json> &id) {
  Json response = {{"jsonrpc", "2.0"}, {"result", result}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return Response(response);
}

Response Response::UserErrorResponse(
    const Json &error, const std::optional<Json> &id) {
  Response response({{"jsonrpc", "2.0"}, {"error", error}}, id);
  response.ValidateResponse();
  return response;
}

void Response::ValidateResponse() const {
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

} // namespace jsonrpc
