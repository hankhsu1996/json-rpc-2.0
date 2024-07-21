#include "json_rpc/core/request.h"

namespace json_rpc {

Request::Request(
    const std::string &method, const Json &params, std::optional<int> id)
    : method_(method), params_(params), id_(id) {
}

Request Request::FromJson(const Json &jsonObj) {
  if (!jsonObj.contains("jsonrpc") || jsonObj["jsonrpc"] != "2.0") {
    throw std::invalid_argument("Invalid JSON-RPC version");
  }
  if (!jsonObj.contains("method")) {
    throw std::invalid_argument("Missing 'method' field");
  }
  if (!jsonObj.contains("params")) {
    throw std::invalid_argument("Missing 'params' field");
  }

  std::optional<int> id =
      jsonObj.contains("id") ? std::optional<int>(jsonObj["id"]) : std::nullopt;
  return Request(jsonObj["method"], jsonObj["params"], id);
}

Json Request::ToJson() const {
  Json jsonObj;
  jsonObj["jsonrpc"] = "2.0";
  jsonObj["method"] = method_;
  jsonObj["params"] = params_;
  if (id_.has_value()) {
    jsonObj["id"] = id_.value();
  }
  return jsonObj;
}

std::string Request::GetMethod() const {
  return method_;
}

Json Request::GetParams() const {
  return params_;
}

std::optional<int> Request::GetId() const {
  return id_;
}

} // namespace json_rpc
