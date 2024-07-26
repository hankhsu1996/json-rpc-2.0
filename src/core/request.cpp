#include "jsonrpc/core/request.hpp"

namespace jsonrpc {

Request Request::FromJson(const Json &jsonObj) {
  auto params = jsonObj.contains("params")
                    ? std::optional<Json>(jsonObj["params"])
                    : std::nullopt;
  auto id = jsonObj.contains("id") ? std::optional<Json>(jsonObj["id"])
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

} // namespace jsonrpc
