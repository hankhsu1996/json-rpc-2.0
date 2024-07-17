#include "json_rpc/core/dispatcher.h"

namespace json_rpc {

std::optional<std::string> Dispatcher::dispatch(
    const std::string &request_str) {
  try {
    json request_json = json::parse(request_str);
    Request request = Request::from_json(request_json);

    auto it = _handlers.find(request.get_method());
    if (it == _handlers.end()) {
      Response error_response = Response::MethodNotFoundError(request.get_id());
      return error_response.to_json().dump();
    }

    const Handler &handler = it->second;
    if (request.get_id().has_value()) {
      if (std::holds_alternative<MethodCallHandler>(handler)) {
        const MethodCallHandler &methodCallHandler =
            std::get<MethodCallHandler>(handler);
        Response response = handleMethodCall(request, methodCallHandler);
        return response.to_json().dump();
      } else {
        Response error_response = Response::InvalidRequestError();
        return error_response.to_json().dump();
      }
    } else {
      if (std::holds_alternative<NotificationHandler>(handler)) {
        const NotificationHandler &notificationHandler =
            std::get<NotificationHandler>(handler);
        handleNotification(request, notificationHandler);
        return std::nullopt;
      } else {
        Response error_response = Response::InvalidRequestError();
        return error_response.to_json().dump();
      }
    }
  } catch (const json::parse_error &) {
    Response error_response = Response::ParseError();
    return error_response.to_json().dump();
  } catch (const std::exception &e) {
    Response error_response = Response::InternalError(std::nullopt);
    return error_response.to_json().dump();
  }
}

Response Dispatcher::handleMethodCall(
    const Request &request, const MethodCallHandler &handler) {
  try {
    json result = handler(request.get_params());
    return Response(result, request.get_id());
  } catch (const std::exception &e) {
    return Response::InternalError(request.get_id());
  }
}

void Dispatcher::handleNotification(
    const Request &request, const NotificationHandler &handler) {
  try {
    handler(request.get_params());
  } catch (const std::exception &) {
    // Notifications do not return errors, but we could log the error if needed
  }
}

void Dispatcher::registerMethodCall(
    const std::string &method, const MethodCallHandler &handler) {
  _handlers[method] = handler;
}

void Dispatcher::registerNotification(
    const std::string &method, const NotificationHandler &handler) {
  _handlers[method] = handler;
}

} // namespace json_rpc
