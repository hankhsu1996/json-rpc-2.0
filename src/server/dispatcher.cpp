#include "jsonrpc/server/dispatcher.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc::server {

Dispatcher::Dispatcher(bool enable_multithreading, size_t num_threads)
    : enable_multithreading_(enable_multithreading),
      thread_pool_(enable_multithreading ? num_threads : 0) {
  // Optionally log or perform additional setup if needed
  if (enable_multithreading_) {
    spdlog::info(
        "Dispatcher initialized with multithreading support using {} threads",
        num_threads);
  } else {
    spdlog::info("Dispatcher initialized without multithreading support");
  }
}

auto Dispatcher::DispatchRequest(const std::string &request_str)
    -> std::optional<std::string> {
  auto request_json = ParseAndValidateJson(request_str);
  if (!request_json.has_value()) {
    return Response::CreateLibError(LibErrorKind::kParseError).ToStr();
  }

  if (request_json->is_array()) {
    return DispatchBatchRequest(*request_json);
  }

  return DispatchSingleRequest(*request_json);
}

auto Dispatcher::ParseAndValidateJson(const std::string &request_str)
    -> std::optional<nlohmann::json> {
  try {
    return nlohmann::json::parse(request_str);
  } catch (const nlohmann::json::parse_error &) {
    spdlog::error("JSON parsing error: {}", request_str);
    return std::nullopt;
  }
}

auto Dispatcher::DispatchSingleRequest(const nlohmann::json &request_json)
    -> std::optional<std::string> {
  auto response_json = DispatchSingleRequestInner(request_json);
  if (response_json.has_value()) {
    return response_json->dump();
  }
  return std::nullopt;
}

auto Dispatcher::DispatchSingleRequestInner(const nlohmann::json &request_json)
    -> std::optional<nlohmann::json> {
  auto validation_error = ValidateRequest(request_json);
  if (validation_error.has_value()) {
    return validation_error->ToJson();
  }

  Request request = Request::FromJson(request_json);
  spdlog::info("Dispatching request: method={}", request.GetMethod());

  auto optional_handler = FindHandler(handlers_, request.GetMethod());
  if (!optional_handler.has_value()) {
    if (request.GetId().has_value()) {
      return Response::CreateLibError(
                 LibErrorKind::kMethodNotFound, request.GetId())
          .ToJson();
    }
    spdlog::warn("Method {} not found for notification", request.GetMethod());
    return std::nullopt;
  }

  return HandleRequest(request, optional_handler.value());
}

auto Dispatcher::DispatchBatchRequest(const nlohmann::json &request_json)
    -> std::optional<std::string> {
  if (request_json.empty()) {
    spdlog::warn("Empty batch request");
    return Response::CreateLibError(LibErrorKind::kInvalidRequest).ToStr();
  }

  auto response_jsons = DispatchBatchRequestInner(request_json);
  if (response_jsons.empty()) {
    return std::nullopt;
  }

  return nlohmann::json(response_jsons).dump();
}

auto Dispatcher::DispatchBatchRequestInner(const nlohmann::json &request_json)
    -> std::vector<nlohmann::json> {
  std::vector<std::future<std::optional<nlohmann::json>>> futures;

  for (const auto &element : request_json) {
    if (enable_multithreading_) {
      futures.emplace_back(thread_pool_.submit_task(
          [this, element]() -> std::optional<nlohmann::json> {
            return DispatchSingleRequestInner(element);
          }));
    } else {
      futures.emplace_back(std::async(
          std::launch::deferred,
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

auto Dispatcher::ValidateRequest(const nlohmann::json &request_json)
    -> std::optional<Response> {
  if (!request_json.is_object()) {
    return Response::CreateLibError(LibErrorKind::kInvalidRequest);
  }

  if (!request_json.contains("jsonrpc") || request_json["jsonrpc"] != "2.0") {
    return Response::CreateLibError(LibErrorKind::kInvalidRequest);
  }

  if (!request_json.contains("method")) {
    if (request_json.contains("id")) {
      // Method call without method field, will return an error
      return Response::CreateLibError(LibErrorKind::kInvalidRequest);
    }
    // Notification without method field, will be ignored
    spdlog::warn("Request missing method field and id");
    return std::nullopt;
  }

  if (!request_json["method"].is_string()) {
    return Response::CreateLibError(LibErrorKind::kInvalidRequest);
  }

  return std::nullopt;  // No errors found
}

auto Dispatcher::FindHandler(
    const std::unordered_map<std::string, Handler> &handlers,
    const std::string &method) -> std::optional<Handler> {
  auto it = handlers.find(method);
  if (it != handlers.end()) {
    return it->second;
  }
  return std::nullopt;
}

auto Dispatcher::HandleRequest(const Request &request, const Handler &handler)
    -> std::optional<nlohmann::json> {
  if (request.GetId().has_value()) {
    // If the request has an ID, it is a method call
    if (std::holds_alternative<MethodCallHandler>(handler)) {
      const auto &method_call_handler = std::get<MethodCallHandler>(handler);
      Response response = HandleMethodCall(request, method_call_handler);
      return response.ToJson();
    }
    return Response::CreateLibError(
               LibErrorKind::kInvalidRequest, request.GetId())
        .ToJson();
  }
  // Otherwise, it is a notification
  if (std::holds_alternative<NotificationHandler>(handler)) {
    const auto &notification_handler = std::get<NotificationHandler>(handler);
    HandleNotification(request, notification_handler);
    return std::nullopt;
  }
  // Invalid request type for a notification
  return std::nullopt;
}

auto Dispatcher::HandleMethodCall(
    const Request &request, const MethodCallHandler &handler) -> Response {
  try {
    nlohmann::json response_json = handler(request.GetParams());
    spdlog::debug(
        "Method call {} returned: {}", request.GetMethod(),
        response_json.dump());

    return Response::FromUserResponse(response_json, request.GetId());
  } catch (const std::exception &e) {
    spdlog::error("Exception during method call handling: {}", e.what());
    return Response::CreateLibError(
        LibErrorKind::kInternalError, request.GetId());
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

}  // namespace jsonrpc::server
