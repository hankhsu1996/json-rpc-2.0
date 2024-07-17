#include "json_rpc/core/dispatcher.h"

namespace json_rpc {

std::optional<std::string> Dispatcher::Dispatch(const std::string &requestStr) {
  try {
    Json requestJson = Json::parse(requestStr);
    Request request = Request::FromJson(requestJson);

    auto it = handlers_.find(request.GetMethod());
    if (it == handlers_.end()) {
      Response errorResponse = Response::MethodNotFoundError(request.GetId());
      return errorResponse.ToJson().dump();
    }

    const Handler &handler = it->second;
    if (request.GetId().has_value()) {
      if (std::holds_alternative<MethodCallHandler>(handler)) {
        const MethodCallHandler &methodCallHandler =
            std::get<MethodCallHandler>(handler);
        Response response = HandleMethodCall(request, methodCallHandler);
        return response.ToJson().dump();
      } else {
        Response errorResponse = Response::InvalidRequestError();
        return errorResponse.ToJson().dump();
      }
    } else {
      if (std::holds_alternative<NotificationHandler>(handler)) {
        const NotificationHandler &notificationHandler =
            std::get<NotificationHandler>(handler);
        HandleNotification(request, notificationHandler);
        return std::nullopt;
      } else {
        Response errorResponse = Response::InvalidRequestError();
        return errorResponse.ToJson().dump();
      }
    }
  } catch (const Json::parse_error &) {
    Response errorResponse = Response::ParseError();
    return errorResponse.ToJson().dump();
  } catch (const std::exception &e) {
    Response errorResponse = Response::InternalError(std::nullopt);
    return errorResponse.ToJson().dump();
  }
}

Response Dispatcher::HandleMethodCall(
    const Request &request, const MethodCallHandler &handler) {
  try {
    Json result = handler(request.GetParams());
    return Response(result, request.GetId());
  } catch (const std::exception &e) {
    return Response::InternalError(request.GetId());
  }
}

void Dispatcher::HandleNotification(
    const Request &request, const NotificationHandler &handler) {
  try {
    handler(request.GetParams());
  } catch (const std::exception &) {
    // Notifications do not return errors, but we could log the error if needed
  }
}

void Dispatcher::RegisterMethodCall(
    const std::string &method, const MethodCallHandler &handler) {
  handlers_[method] = handler;
}

void Dispatcher::RegisterNotification(
    const std::string &method, const NotificationHandler &handler) {
  handlers_[method] = handler;
}

} // namespace json_rpc
