#include "jsonrpc/server/response.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace server {

const ErrorInfoMap Response::errorInfoMap = {
    {LibErrorKind::ParseError, {-32700, "Parse error"}},
    {LibErrorKind::InvalidRequest, {-32600, "Invalid Request"}},
    {LibErrorKind::MethodNotFound, {-32601, "Method not found"}},
    {LibErrorKind::InternalError, {-32603, "Internal error"}},
    {LibErrorKind::ServerError, {-32000, "Server error"}}};

Response::Response(Response &&other) noexcept
    : response_(std::move(other.response_)) {
}

Response::Response(nlohmann::json response, std::optional<nlohmann::json> id)
    : response_(std::move(response)) {
  if (id.has_value()) {
    response_["id"] = id.value();
  }
  ValidateResponse();
}

Response Response::FromUserResponse(
    const nlohmann::json &responseJson, std::optional<nlohmann::json> id) {
  if (responseJson.contains("result")) {
    return CreateResult(responseJson["result"], id);
  } else if (responseJson.contains("error")) {
    return CreateUserError(responseJson["error"], id);
  } else {
    throw std::invalid_argument(
        "Response JSON must contain either 'result' or 'error' field");
  }
}

Response Response::CreateResult(
    const nlohmann::json &result, const std::optional<nlohmann::json> &id) {
  nlohmann::json response = {{"jsonrpc", "2.0"}, {"result", result}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return Response{std::move(response)};
}

Response Response::CreateUserError(
    const nlohmann::json &error, const std::optional<nlohmann::json> &id) {
  nlohmann::json response = {{"jsonrpc", "2.0"}, {"error", error}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return Response{std::move(response)};
}

nlohmann::json Response::ToJson() const {
  return response_;
}

std::string Response::ToStr() const {
  return response_.dump();
}

Response Response::CreateLibError(
    LibErrorKind errorKind, const std::optional<nlohmann::json> &id) {
  const auto &[code, message] = errorInfoMap.at(errorKind);
  nlohmann::json errorResponse = CreateErrorResponse(message, code, id);
  return Response{std::move(errorResponse)};
}

nlohmann::json Response::CreateErrorResponse(const std::string &message,
    int code, const std::optional<nlohmann::json> &id) {
  nlohmann::json error = {{"code", code}, {"message", message}};
  nlohmann::json response = {{"jsonrpc", "2.0"}, {"error", error}};
  if (id.has_value()) {
    response["id"] = id.value();
  } else {
    response["id"] = nullptr;
  }
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

} // namespace server
} // namespace jsonrpc
