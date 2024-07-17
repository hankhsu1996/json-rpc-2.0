#include "json_rpc/client/client.h"

namespace json_rpc {

Client::Client(std::unique_ptr<Transport> transport)
    : _transport(std::move(transport)) {
}

Client::~Client() = default;

nlohmann::json Client::sendMethodCall(
    const std::string &method, const nlohmann::json &params, int id) {
  // Create the JSON-RPC request
  Request request(method, params, id);
  nlohmann::json request_json = request.to_json();

  // Send the request and wait for the response
  return _transport->sendMethodCall(request_json);
}

void Client::sendNotification(
    const std::string &method, const nlohmann::json &params) {
  // Create the JSON-RPC notification (id is not set)
  Request notification(method, params, std::nullopt);
  nlohmann::json notification_json = notification.to_json();

  // Send the notification
  _transport->sendNotification(notification_json);
}

} // namespace json_rpc
