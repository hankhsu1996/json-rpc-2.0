#include "json_rpc/core/dispatcher.h"

namespace json_rpc {

std::optional<nlohmann::json> Dispatcher::dispatch(
    const nlohmann::json &request_json) {
  try {
    Request request = Request::from_json(request_json);
    std::string method = request.get_method();
    auto it = _handlers.find(method);

    if (it != _handlers.end()) {
      if (request.get_id().has_value()) {
        // It's a method call
        if (std::holds_alternative<JsonRpcMethodHandler>(it->second)) {
          JsonRpcMethodHandler handler =
              std::get<JsonRpcMethodHandler>(it->second);
          Response response = handler(request);
          return response.to_json();
        } else {
          Error error(Error::METHOD_NOT_FOUND, "Method not found");
          Response response(error, request.get_id());
          return response.to_json();
        }
      } else {
        // It's a notification
        if (std::holds_alternative<JsonRpcNotificationHandler>(it->second)) {
          JsonRpcNotificationHandler handler =
              std::get<JsonRpcNotificationHandler>(it->second);
          handler(request);
          return std::nullopt; // No response for notifications
        } else {
          Error error(Error::METHOD_NOT_FOUND, "Method not found");
          Response response(error, std::nullopt);
          return response.to_json();
        }
      }
    } else {
      if (request.get_id().has_value()) {
        Error error(Error::METHOD_NOT_FOUND, "Method not found");
        Response response(error, request.get_id());
        return response.to_json();
      }
    }
  } catch (const std::exception &e) {
    Error error(Error::INTERNAL_ERROR, e.what());
    Response response(error, std::nullopt);
    return response.to_json();
  }

  return std::nullopt;
}

void Dispatcher::registerMethod(
    const std::string &method, const JsonRpcMethodHandler &handler) {
  _handlers[method] = handler;
}

void Dispatcher::registerNotification(
    const std::string &method, const JsonRpcNotificationHandler &handler) {
  _handlers[method] = handler;
}

} // namespace json_rpc
