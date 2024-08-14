#include "jsonrpc/client/request.hpp"

namespace jsonrpc::client {

Request::Request(
    std::string method, std::optional<nlohmann::json> params,
    bool is_notification, const std::function<int()> &id_generator)
    : method_(std::move(method)),
      params_(std::move(params)),
      is_notification_(is_notification),
      id_(0) {
  if (!is_notification_) {
    id_ = id_generator();
  }
}

auto Request::RequiresResponse() const -> bool {
  return !is_notification_;
}

auto Request::GetKey() const -> int {
  return id_;
}

auto Request::Dump() const -> std::string {
  nlohmann::json json_request;
  json_request["jsonrpc"] = "2.0";
  json_request["method"] = method_;
  if (params_) {
    json_request["params"] = *params_;
  }
  if (!is_notification_) {
    json_request["id"] = id_;
  }
  return json_request.dump();
}

}  // namespace jsonrpc::client
