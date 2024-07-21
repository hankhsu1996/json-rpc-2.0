#include <spdlog/spdlog.h>

#include "json_rpc/core/dispatcher.h"

namespace json_rpc {

std::optional<std::string> Dispatcher::Dispatch(const std::string &requestStr) {
  try {
    // Parse the request
    Json requestJson = Json::parse(requestStr);
    Request request = Request::FromJson(requestJson);

    spdlog::info("Dispatching request: method={}", request.GetMethod());

    // Find the handler for the method
    auto it = handlers_.find(request.GetMethod());
    if (it == handlers_.end()) {
      Response errorResponse = Response::MethodNotFoundError(request.GetId());
      return errorResponse.ToJson().dump();
    }

    const Handler &handler = it->second;
    if (request.GetId().has_value()) {
      // If the request has an ID, it is a method call
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
      // Otherwise, it is a notification
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
    spdlog::error("Exception caught during dispatch: {}", e.what());
    Response errorResponse = Response::InternalError(std::nullopt);
    return errorResponse.ToJson().dump();
  }
}

Response Dispatcher::HandleMethodCall(
    const Request &request, const MethodCallHandler &handler) {
  try {
    Json result = handler(request.GetParams());

    if (spdlog::should_log(spdlog::level::info)) {
      if (result.contains("result")) {
        spdlog::info("Method call {} handled successfully with result: {}",
            request.GetMethod(), result["result"].dump());
      } else if (result.contains("error")) {
        spdlog::info("Method call {} returned error: {}", request.GetMethod(),
            result["error"].dump());
      }
    }

    return Response(result, request.GetId());
  } catch (const std::exception &e) {
    spdlog::error("Exception during method call handling: {}", e.what());
    return Response::InternalError(request.GetId());
  }
}

void Dispatcher::HandleNotification(
    const Request &request, const NotificationHandler &handler) {
  try {
    handler(request.GetParams());
    spdlog::info("Notification {} handled successfully", request.GetMethod());
  } catch (const std::exception &e) {
    spdlog::error("Exception during notification handling: {}", e.what());
  }
}

void Dispatcher::RegisterMethodCall(
    const std::string &method, const MethodCallHandler &handler) {
  handlers_[method] = handler;
  spdlog::info("Dispatcher registered method call: {}", method);
}

void Dispatcher::RegisterNotification(
    const std::string &method, const NotificationHandler &handler) {
  handlers_[method] = handler;
  spdlog::info("Dispatcher registered notification: {}", method);
}

} // namespace json_rpc
