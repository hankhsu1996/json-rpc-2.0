#include "json_rpc/client/client.h"

namespace json_rpc {

Client::Client(std::unique_ptr<Transport> transport)
    : _transport(std::move(transport)) {
}

Client::~Client() = default;

Response Client::sendRequest(
    const std::string &method, const nlohmann::json &params, int id) {
  Request request(method, params, id);
  nlohmann::json request_json = request.to_json();
  nlohmann::json response_json = _transport->sendRequest(request_json);
  return Response::from_json(response_json);
}

void Client::sendNotification(
    const std::string &method, const nlohmann::json &params) {
  Request request(method, params, std::nullopt);
  nlohmann::json request_json = request.to_json();
  _transport->sendNotification(request_json);
}

} // namespace json_rpc
