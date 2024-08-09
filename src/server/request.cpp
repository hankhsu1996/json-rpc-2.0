#include "jsonrpc/server/request.hpp"

namespace jsonrpc::server {

Request::Request(
    std::string method, const std::optional<nlohmann::json> &params,
    std::optional<nlohmann::json> id)
    : method_(std::move(method)), params_(params), id_(std::move(id)) {
}

auto Request::FromJson(const nlohmann::json &json_obj) -> Request {
  auto params = json_obj.contains("params")
                    ? std::optional<nlohmann::json>(json_obj["params"])
                    : std::nullopt;

  auto id = json_obj.contains("id")
                ? std::optional<nlohmann::json>(json_obj["id"])
                : std::nullopt;
  return Request(json_obj["method"], params, id);
}

auto Request::ToJson() const -> nlohmann::json {
  nlohmann::json json_obj;
  json_obj["jsonrpc"] = "2.0";
  json_obj["method"] = method_;
  if (params_.has_value()) {
    json_obj["params"] = params_.value();
  }
  if (id_.has_value()) {
    json_obj["id"] = id_.value();
  }
  return json_obj;
}

}  // namespace jsonrpc::server
