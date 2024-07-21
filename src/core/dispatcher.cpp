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
      Response errorResponse = Response::LibraryErrorResponse(
          "Method not found", -32601, request.GetId());
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
        Response errorResponse =
            Response::LibraryErrorResponse("Invalid Request", -32600);
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
        Response errorResponse =
            Response::LibraryErrorResponse("Invalid Request", -32600);
        return errorResponse.ToJson().dump();
      }
    }
  } catch (const Json::parse_error &) {
    Response errorResponse =
        Response::LibraryErrorResponse("Parse error", -32700);
    return errorResponse.ToJson().dump();
  } catch (const std::exception &e) {
    spdlog::error("Exception caught during dispatch: {}", e.what());
    Response errorResponse =
        Response::LibraryErrorResponse("Internal error", -32603);
    return errorResponse.ToJson().dump();
  }
}

Response Dispatcher::HandleMethodCall(
    const Request &request, const MethodCallHandler &handler) {
  try {
    Json responseJson = handler(request.GetParams());

    spdlog::debug("Method call {} returned: {}", request.GetMethod(),
        responseJson.dump());

    return Response::FromJson(responseJson, request.GetId());
  } catch (const std::exception &e) {
    spdlog::error("Exception during method call handling: {}", e.what());
    return Response::LibraryErrorResponse(
        "Internal error", -32603, request.GetId());
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
