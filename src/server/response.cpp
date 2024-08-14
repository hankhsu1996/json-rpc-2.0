#include "jsonrpc/server/response.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc::server {

const ErrorInfoMap Response::kErrorInfoMap = {
    {LibErrorKind::kParseError, {-32700, "Parse error"}},
    {LibErrorKind::kInvalidRequest, {-32600, "Invalid Request"}},
    {LibErrorKind::kMethodNotFound, {-32601, "Method not found"}},
    {LibErrorKind::kInternalError, {-32603, "Internal error"}},
    {LibErrorKind::kServerError, {-32000, "Server error"}}};

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

auto Response::FromUserResponse(
    const nlohmann::json &response_json,
    std::optional<nlohmann::json> id) -> Response {
  if (response_json.contains("result")) {
    return CreateResult(response_json["result"], id);
  }
  if (response_json.contains("error")) {
    return CreateUserError(response_json["error"], id);
  }
  throw std::invalid_argument(
      "Response JSON must contain either 'result' or 'error' field");
}

auto Response::CreateResult(
    const nlohmann::json &result,
    const std::optional<nlohmann::json> &id) -> Response {
  nlohmann::json response = {{"jsonrpc", "2.0"}, {"result", result}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return Response{std::move(response)};
}

auto Response::CreateUserError(
    const nlohmann::json &error,
    const std::optional<nlohmann::json> &id) -> Response {
  nlohmann::json response = {{"jsonrpc", "2.0"}, {"error", error}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return Response{std::move(response)};
}

auto Response::ToJson() const -> nlohmann::json {
  return response_;
}

auto Response::ToStr() const -> std::string {
  return response_.dump();
}

auto Response::CreateLibError(
    LibErrorKind error_kind,
    const std::optional<nlohmann::json> &id) -> Response {
  const auto &[code, message] = kErrorInfoMap.at(error_kind);
  nlohmann::json error_response = CreateErrorResponse(message, code, id);
  return Response{std::move(error_response)};
}

auto Response::CreateErrorResponse(
    const std::string &message, int code,
    const std::optional<nlohmann::json> &id) -> nlohmann::json {
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
      spdlog::error(
          "Response validation failed: missing 'code' or 'message' "
          "in error object");
      throw std::invalid_argument(
          "Error object must contain 'code' and 'message' fields.");
    }
  }
}

}  // namespace jsonrpc::server
