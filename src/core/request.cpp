#include "json_rpc/core/request.h"

namespace json_rpc {

Request::Request(const std::string &method, const nlohmann::json &params,
    std::optional<int> id)
    : _method(method), _params(params), _id(id) {
}

Request Request::from_json(const nlohmann::json &json_obj) {
  std::optional<int> id = json_obj.contains("id")
                              ? std::optional<int>(json_obj["id"])
                              : std::nullopt;
  return Request(json_obj["method"], json_obj["params"], id);
}

nlohmann::json Request::to_json() const {
  nlohmann::json json_obj;
  json_obj["method"] = _method;
  json_obj["params"] = _params;
  if (_id.has_value()) {
    json_obj["id"] = _id.value();
  }
  return json_obj;
}

std::string Request::get_method() const {
  return _method;
}

nlohmann::json Request::get_params() const {
  return _params;
}

std::optional<int> Request::get_id() const {
  return _id;
}

} // namespace json_rpc
