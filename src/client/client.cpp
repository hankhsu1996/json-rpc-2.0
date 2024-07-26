#include "jsonrpc/client/client.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {

Client::Client(std::unique_ptr<ClientTransport> transport)
    : transport_(std::move(transport)) {
  spdlog::info("Client initialized with transport");
}

Json Client::SendMethodCall(
    const std::string &method, const Json &params, int id) {
  spdlog::info("Client sending method call: {}", method);

  // Create the JSON-RPC request
  Request request(method, params, id);
  Json requestJson = request.ToJson();

  // Send the request and wait for the response
  return transport_->SendMethodCall(requestJson);
}

void Client::SendNotification(const std::string &method, const Json &params) {
  spdlog::info("Client sending notification: {}", method);

  // Create the JSON-RPC notification (id is not set)
  Request notification(method, params, std::nullopt);
  Json notificationJson = notification.ToJson();

  // Send the notification
  transport_->SendNotification(notificationJson);
}

} // namespace jsonrpc
