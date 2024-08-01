#include "jsonrpc/client/request.hpp"

namespace jsonrpc {

ClientRequest::ClientRequest(const std::string &method,
    std::optional<nlohmann::json> params, bool isNotification,
    const std::function<int()> &idGenerator)
    : method_(method), params_(std::move(params)),
      isNotification_(isNotification), id_(0) {
  if (!isNotification_) {
    id_ = idGenerator();
  }
}

bool ClientRequest::RequiresResponse() const {
  return !isNotification_;
}

int ClientRequest::GetKey() const {
  return id_;
}

std::string ClientRequest::Dump() const {
  nlohmann::json jsonRequest;
  jsonRequest["jsonrpc"] = "2.0";
  jsonRequest["method"] = method_;
  if (params_) {
    jsonRequest["params"] = *params_;
  }
  if (!isNotification_) {
    jsonRequest["id"] = id_;
  }
  return jsonRequest.dump();
}

} // namespace jsonrpc
