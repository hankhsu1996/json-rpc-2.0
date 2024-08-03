#include "jsonrpc/server/dispatcher.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace server {

Dispatcher::Dispatcher(bool enableMultithreading, size_t numThreads)
    : enableMultithreading_(enableMultithreading),
      thread_pool_(enableMultithreading ? numThreads : 0) {
  // Optionally log or perform additional setup if needed
  if (enableMultithreading_) {
    spdlog::info(
        "Dispatcher initialized with multithreading support using {} threads",
        numThreads);
  } else {
    spdlog::info("Dispatcher initialized without multithreading support");
  }
}

std::optional<std::string> Dispatcher::DispatchRequest(
    const std::string &requestStr) {

  auto requestJson = ParseAndValidateJson(requestStr);
  if (!requestJson.has_value()) {
    return Response::CreateLibError(LibErrorKind::ParseError).ToStr();
  }

  if (requestJson->is_array()) {
    return DispatchBatchRequest(*requestJson);
  } else {
    return DispatchSingleRequest(*requestJson);
  }
}

std::optional<nlohmann::json> Dispatcher::ParseAndValidateJson(
    const std::string &requestStr) {
  try {
    return nlohmann::json::parse(requestStr);
  } catch (const nlohmann::json::parse_error &) {
    spdlog::error("JSON parsing error: {}", requestStr);
    return std::nullopt;
  }
}

std::optional<std::string> Dispatcher::DispatchSingleRequest(
    const nlohmann::json &requestJson) {
  std::optional<nlohmann::json> responseJson =
      DispatchSingleRequestInner(requestJson);
  if (responseJson.has_value()) {
    return responseJson->dump();
  }
  return std::nullopt;
}

std::optional<nlohmann::json> Dispatcher::DispatchSingleRequestInner(
    const nlohmann::json &requestJson) {
  auto validationError = ValidateRequest(requestJson);
  if (validationError.has_value()) {
    return validationError->ToJson();
  }

  Request request = Request::FromJson(requestJson);
  spdlog::info("Dispatching request: method={}", request.GetMethod());

  auto optionalHandler = FindHandler(handlers_, request.GetMethod());
  if (!optionalHandler.has_value()) {
    if (request.GetId().has_value()) {
      return Response::CreateLibError(
          LibErrorKind::MethodNotFound, request.GetId())
          .ToJson();
    } else {
      spdlog::warn("Method {} not found for notification", request.GetMethod());
      return std::nullopt;
    }
  }

  return HandleRequest(request, optionalHandler.value());
}

std::optional<std::string> Dispatcher::DispatchBatchRequest(
    const nlohmann::json &requestJson) {
  if (requestJson.empty()) {
    spdlog::warn("Empty batch request");
    return Response::CreateLibError(LibErrorKind::InvalidRequest).ToStr();
  }

  std::vector<nlohmann::json> responseJsons =
      DispatchBatchRequestInner(requestJson);
  if (responseJsons.empty()) {
    return std::nullopt;
  }

  return nlohmann::json(responseJsons).dump();
}

std::vector<nlohmann::json> Dispatcher::DispatchBatchRequestInner(
    const nlohmann::json &requestJson) {
  std::vector<std::future<std::optional<nlohmann::json>>> futures;

  for (const auto &element : requestJson) {
    if (enableMultithreading_) {
      futures.emplace_back(thread_pool_.submit_task(
          [this, element]() -> std::optional<nlohmann::json> {
            return DispatchSingleRequestInner(element);
          }));
    } else {
      futures.emplace_back(std::async(std::launch::deferred,
          [this, element]() -> std::optional<nlohmann::json> {
            return DispatchSingleRequestInner(element);
          }));
    }
  }

  std::vector<nlohmann::json> responses;
  for (auto &future : futures) {
    std::optional<nlohmann::json> response = future.get();
    if (response.has_value()) {
      responses.push_back(response.value());
    }
  }

  return responses;
}

std::optional<Response> Dispatcher::ValidateRequest(
    const nlohmann::json &requestJson) {
  if (!requestJson.is_object()) {
    return Response::CreateLibError(LibErrorKind::InvalidRequest);
  }

  if (!requestJson.contains("jsonrpc") || requestJson["jsonrpc"] != "2.0") {
    return Response::CreateLibError(LibErrorKind::InvalidRequest);
  }

  if (!requestJson.contains("method")) {
    if (requestJson.contains("id")) {
      // Method call without method field, will return an error
      return Response::CreateLibError(LibErrorKind::InvalidRequest);
    } else {
      // Notification without method field, will be ignored
      spdlog::warn("Request missing method field and id");
      return std::nullopt;
    }
  } else if (!requestJson["method"].is_string()) {
    return Response::CreateLibError(LibErrorKind::InvalidRequest);
  }

  return std::nullopt; // No errors found
}

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

std::optional<nlohmann::json> Dispatcher::HandleRequest(
    const Request &request, const Handler &handler) {
  if (request.GetId().has_value()) {
    // If the request has an ID, it is a method call
    if (std::holds_alternative<MethodCallHandler>(handler)) {
      const MethodCallHandler &methodCallHandler =
          std::get<MethodCallHandler>(handler);
      Response response = HandleMethodCall(request, methodCallHandler);
      return response.ToJson();
    } else {
      return Response::CreateLibError(
          LibErrorKind::InvalidRequest, request.GetId())
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
    nlohmann::json responseJson = handler(request.GetParams());
    spdlog::debug("Method call {} returned: {}", request.GetMethod(),
        responseJson.dump());

    return Response::FromUserResponse(responseJson, request.GetId());
  } catch (const std::exception &e) {
    spdlog::error("Exception during method call handling: {}", e.what());
    return Response::CreateLibError(
        LibErrorKind::InternalError, request.GetId());
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

} // namespace server
} // namespace jsonrpc
