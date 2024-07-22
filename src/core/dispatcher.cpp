#include <spdlog/spdlog.h>

#include "json_rpc/core/dispatcher.hpp"

namespace json_rpc {

std::optional<std::string> Dispatcher::DispatchRequest(
    const std::string &requestStr) {
  try {
    Json requestJson = Json::parse(requestStr);
    if (requestJson.is_array()) {
      std::vector<Json> responseJsons = DispatchBatchRequest(requestJson);
      Json responseJson = responseJsons;
      return responseJson.dump();
    } else if (requestJson.is_object()) {
      std::optional<Json> responseJson = DispatchSingleRequest(requestJson);
      if (responseJson.has_value()) {
        return responseJson->dump();
      }
      return std::nullopt;
    } else {
      Response errorResponse =
          Response::LibraryErrorResponse("Parse error", -32700);
      return errorResponse.ToJson().dump();
    }
  } catch (const Json::parse_error &) {
    Response errorResponse =
        Response::LibraryErrorResponse("Parse error", -32700);
    return errorResponse.ToJson().dump();
  }
}

std::optional<Json> Dispatcher::DispatchSingleRequest(const Json &requestJson) {
  try {
    // Step 1: Validate the request
    std::optional<Response> validationError = ValidateRequest(requestJson);
    if (validationError.has_value()) {
      return validationError->ToJson();
    }

    // Step 2: Create the Request object
    Request request = Request::FromJson(requestJson);
    spdlog::info("Dispatching request: method={}", request.GetMethod());

    // Step 3: Find the handler for the method
    std::optional<Handler> handlerOpt =
        FindHandler(handlers_, request.GetMethod());
    if (!handlerOpt.has_value()) {
      if (request.GetId().has_value()) {
        // Method not found for a method call
        return Response::LibraryErrorResponse(
            "Method not found", -32601, request.GetId())
            .ToJson();
      } else {
        // Method not found for a notification, return std::nullopt
        spdlog::warn("Method not found for notification: method={}",
            request.GetMethod());
        return std::nullopt;
      }
    }

    // Step 4: Handle the request
    return HandleRequest(request, handlerOpt.value());

  } catch (const Json::parse_error &) {
    return Response::LibraryErrorResponse("Parse error", -32700).ToJson();
  } catch (const std::exception &e) {
    spdlog::error("Exception caught during dispatch: {}", e.what());
    return Response::LibraryErrorResponse("Internal error", -32603).ToJson();
  }
}

std::vector<Json> Dispatcher::DispatchBatchRequest(const Json &requestJson) {
  std::vector<std::future<std::optional<Json>>> futures;
  for (const auto &element : requestJson) {
    futures.emplace_back(
        thread_pool_.submit_task([this, element]() -> std::optional<Json> {
          try {
            if (!element.is_object()) {
              Response errorResponse =
                  Response::LibraryErrorResponse("Invalid Request", -32600);
              return errorResponse.ToJson();
            }
            return DispatchSingleRequest(element);
          } catch (const Json::parse_error &) {
            Response errorResponse =
                Response::LibraryErrorResponse("Parse error", -32700);
            return errorResponse.ToJson();
          }
        }));
  }

  std::vector<Json> responses;
  for (auto &future : futures) {
    std::optional<Json> response = future.get();
    if (response.has_value()) {
      responses.push_back(response.value());
    }
  }

  return responses;
}

// Function to validate the JSON request
std::optional<Response> Dispatcher::ValidateRequest(const Json &requestJson) {
  if (!requestJson.contains("jsonrpc") || requestJson["jsonrpc"] != "2.0") {
    return Response::LibraryErrorResponse("Invalid Request", -32600);
  }

  if (!requestJson.contains("method")) {
    if (requestJson.contains("id")) {
      // No method and has id
      return Response::LibraryErrorResponse("Invalid Request", -32600);
    } else {
      // No method and no id
      spdlog::warn("Request missing method field and id");
      return std::nullopt;
    }
  } else if (!requestJson["method"].is_string()) {
    // Method is not a string
    return Response::LibraryErrorResponse("Invalid Request", -32600);
  }

  return std::nullopt; // No errors found
}

// Function to find the handler for the method
std::optional<Handler> Dispatcher::FindHandler(
    const std::unordered_map<std::string, Handler> &handlers,
    const std::string &method) {
  auto it = handlers.find(method);
  if (it != handlers.end()) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

// Function to handle method call or notification
std::optional<Json> Dispatcher::HandleRequest(
    const Request &request, const Handler &handler) {
  if (request.GetId().has_value()) {
    // If the request has an ID, it is a method call
    if (std::holds_alternative<MethodCallHandler>(handler)) {
      const MethodCallHandler &methodCallHandler =
          std::get<MethodCallHandler>(handler);
      Response response = HandleMethodCall(request, methodCallHandler);
      return response.ToJson();
    } else {
      return Response::LibraryErrorResponse(
          "Invalid Request", -32600, request.GetId())
          .ToJson();
    }
  } else {
    // Otherwise, it is a notification
    if (std::holds_alternative<NotificationHandler>(handler)) {
      const NotificationHandler &notificationHandler =
          std::get<NotificationHandler>(handler);
      HandleNotification(request, notificationHandler);
      return std::nullopt;
    } else {
      // Invalid request type for a notification
      return std::nullopt;
    }
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
