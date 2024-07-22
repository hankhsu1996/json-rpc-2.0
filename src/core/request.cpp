#include "json_rpc/core/request.hpp"

namespace json_rpc {

Request::Request(const std::string &method, const std::optional<Json> &params,
    std::optional<Json> id)
    : method_(method), params_(params), id_(id) {
}

Request Request::FromJson(const Json &jsonObj) {
  std::optional<Json> params = jsonObj.contains("params")
                                   ? std::optional<Json>(jsonObj["params"])
                                   : std::nullopt;
  std::optional<Json> id = jsonObj.contains("id")
                               ? std::optional<Json>(jsonObj["id"])
                               : std::nullopt;
  return Request(jsonObj["method"], params, id);
}

Json Request::ToJson() const {
  Json jsonObj;
  jsonObj["jsonrpc"] = "2.0";
  jsonObj["method"] = method_;
  if (params_.has_value()) {
    jsonObj["params"] = params_.value();
  }
  if (id_.has_value()) {
    jsonObj["id"] = id_.value();
  }
  return jsonObj;
}

std::string Request::GetMethod() const {
  return method_;
}

std::optional<Json> Request::GetParams() const {
  return params_;
}

std::optional<Json> Request::GetId() const {
  return id_;
}

} // namespace json_rpc
