#include "json_rpc/core/response.h"

namespace json_rpc {

Response::Response(const nlohmann::json &result, std::optional<int> id)
    : _result(result), _error(Error(0, "")), _id(id), _success(true) {
}

Response::Response(const Error &error, std::optional<int> id)
    : _result(nullptr), _error(error), _id(id), _success(false) {
}

Response Response::from_json(const nlohmann::json &json_obj) {
  if (json_obj.contains("error")) {
    Error error = Error::from_json(json_obj["error"]);
    return Response(error, json_obj.contains("id")
                               ? std::optional<int>(json_obj["id"])
                               : std::nullopt);
  } else {
    nlohmann::json result = json_obj["result"];
    return Response(result, json_obj.contains("id")
                                ? std::optional<int>(json_obj["id"])
                                : std::nullopt);
  }
}

nlohmann::json Response::to_json() const {
  nlohmann::json json_obj;
  json_obj["jsonrpc"] = "2.0";
  if (_success) {
    json_obj["result"] = _result;
  } else {
    json_obj["error"] = _error.to_json();
  }
  if (_id.has_value()) {
    json_obj["id"] = _id.value();
  } else {
    json_obj["id"] = nullptr;
  }
  return json_obj;
}

nlohmann::json Response::get_result() const {
  return _result;
}

Error Response::get_error() const {
  return _error;
}

std::optional<int> Response::get_id() const {
  return _id;
}

bool Response::is_success() const {
  return _success;
}

} // namespace json_rpc
