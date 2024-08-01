#include "jsonrpc/client/client.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {

Client::Client(std::unique_ptr<ClientTransport> transport)
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

void Client::Listener() {
  while (running_) {
    spdlog::debug("JSON-RPC client listener running and expecting {} responses",
        expectedResponses_);
    if (expectedResponses_ > 0) {
      std::string response = transport_->ReadResponse();
      HandleResponse(response);
      expectedResponses_--;
    }
  }
}

nlohmann::json Client::SendMethodCall(
    const std::string &method, std::optional<nlohmann::json> params) {
  ClientRequest request(method, std::move(params), false,
      [this]() { return GetNextRequestId(); });
  return SendRequest(request);
}

void Client::SendNotification(
    const std::string &method, std::optional<nlohmann::json> params) {
  ClientRequest request(
      method, std::move(params), true, [this]() { return GetNextRequestId(); });
  // Notifications do not expect a response
  transport_->SendRequest(request.Dump());
}

nlohmann::json Client::SendRequest(const ClientRequest &request) {
  if (request.RequiresResponse()) {
    std::promise<nlohmann::json> responsePromise;
    auto futureResponse = responsePromise.get_future();

    {
      std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
      pendingRequests_[request.GetKey()] = std::move(responsePromise);
    }
    expectedResponses_++;

    transport_->SendRequest(request.Dump());

    // Block until the promise is fulfilled and return the response
    futureResponse.wait();
    auto response = futureResponse.get();

    {
      std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
      pendingRequests_.erase(request.GetKey());
    }

    return response;
  } else {
    // No response expected, just send the request
    transport_->SendRequest(request.Dump());
    return nlohmann::json(); // Return an empty JSON object since no response is
                             // expected
  }
}

int Client::GetNextRequestId() {
  return requestCounter_++;
}

void Client::HandleResponse(const std::string &response) {
  Json jsonResponse;
  try {
    jsonResponse = Json::parse(response);
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

bool Client::ValidateResponse(const Json &response) {
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
    const Json &error = response["error"];
    if (!error.contains("code") || !error["code"].is_number() ||
        !error.contains("message") || !error["message"].is_string()) {
      return false;
    }
  }

  return true;
}

} // namespace jsonrpc
