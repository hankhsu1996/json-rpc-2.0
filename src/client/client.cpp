#include "jsonrpc/client/client.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace client {

Client::Client(std::unique_ptr<transports::ClientTransport> transport)
    : transport_(std::move(transport)) {
  spdlog::info("Initializing JSON-RPC client");
}

void Client::Start() {
  spdlog::info("Starting JSON-RPC client");
  running_ = true;
  listenerThread_ = std::thread(&Client::Listener, this);
}

void Client::Stop() {
  spdlog::info("Stopping JSON-RPC client");
  running_ = false;
  if (listenerThread_.joinable()) {
    listenerThread_.join();
  }
}

bool Client::isRunning() const {
  return running_;
}

bool Client::HasPendingRequests() const {
  std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
  return !pendingRequests_.empty();
}

void Client::Listener() {
  while (running_) {
    if (expectedResponses_ > 0) {
      std::string response = transport_->ReadResponse();
      HandleResponse(response);
      expectedResponses_--;
    }
  }
}

nlohmann::json Client::SendMethodCall(
    const std::string &method, std::optional<nlohmann::json> params) {
  Request request(method, std::move(params), false,
      [this]() { return GetNextRequestId(); });
  return SendRequest(request);
}

std::future<nlohmann::json> Client::SendMethodCallAsync(
    const std::string &method, std::optional<nlohmann::json> params) {
  Request request(method, std::move(params), false,
      [this]() { return GetNextRequestId(); });
  return SendRequestAsync(request);
}

void Client::SendNotification(
    const std::string &method, std::optional<nlohmann::json> params) {
  Request request(
      method, std::move(params), true, [this]() { return GetNextRequestId(); });
  transport_->SendRequest(request.Dump());
}

nlohmann::json Client::SendRequest(const Request &request) {
  auto futureResponse = SendRequestAsync(request);
  return futureResponse.get();
}

std::future<nlohmann::json> Client::SendRequestAsync(const Request &request) {
  assert(request.RequiresResponse() && "SendRequestAsync called for a request "
                                       "that does not require a response.");

  std::promise<nlohmann::json> responsePromise;
  auto futureResponse = responsePromise.get_future();

  {
    std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
    pendingRequests_[request.GetKey()] = std::move(responsePromise);
  }
  expectedResponses_++;

  transport_->SendRequest(request.Dump());

  return futureResponse;
}

int Client::GetNextRequestId() {
  return requestCounter_++;
}

void Client::HandleResponse(const std::string &response) {
  nlohmann::json jsonResponse;
  try {
    jsonResponse = nlohmann::json::parse(response);
  } catch (const std::exception &e) {
    spdlog::error("Failed to parse JSON response: {}", e.what());
    throw std::runtime_error(
        "Failed to parse JSON response: " + std::string(e.what()));
  }
  if (ValidateResponse(jsonResponse)) {
    int requestId = jsonResponse["id"].get<int>();

    std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
    auto it = pendingRequests_.find(requestId);
    if (it != pendingRequests_.end()) {
      it->second.set_value(jsonResponse);
      pendingRequests_.erase(it);
    } else {
      spdlog::error("Received response for unknown request ID: {}", requestId);
      throw std::runtime_error("Received response for unknown request ID: " +
                               std::to_string(requestId));
    }
  } else {
    spdlog::error("Invalid JSON-RPC response: {}", jsonResponse.dump());
    throw std::runtime_error(
        "Invalid JSON-RPC response: " + jsonResponse.dump());
  }
}

bool Client::ValidateResponse(const nlohmann::json &response) {
  if (!response.contains("jsonrpc") || response["jsonrpc"] != "2.0") {
    return false;
  }

  if (!response.contains("id")) {
    return false;
  }

  bool hasResult = response.contains("result");
  bool hasError = response.contains("error");

  if (hasResult == hasError) { // Mutually exclusive
    return false;
  }

  if (hasError) {
    const nlohmann::json &error = response["error"];
    if (!error.contains("code") || !error["code"].is_number() ||
        !error.contains("message") || !error["message"].is_string()) {
      return false;
    }
  }

  return true;
}

} // namespace client
} // namespace jsonrpc
