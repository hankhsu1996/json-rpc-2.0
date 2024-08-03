#include "jsonrpc/server/request.hpp"

namespace jsonrpc {
namespace server {

Request Request::FromJson(const nlohmann::json &jsonObj) {
  auto params = jsonObj.contains("params")
                    ? std::optional<nlohmann::json>(jsonObj["params"])
                    : std::nullopt;
  auto id = jsonObj.contains("id")
                ? std::optional<nlohmann::json>(jsonObj["id"])
                : std::nullopt;
  return Request(jsonObj["method"], params, id);
}

nlohmann::json Request::ToJson() const {
  nlohmann::json jsonObj;
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

} // namespace server
} // namespace jsonrpc
