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

Client::Response Client::SendRequest(const Request &request) {
  const std::string &method = std::get<0>(request);
  const std::optional<Json> &params = std::get<1>(request);
  bool isNotification = std::get<2>(request);

  Json jsonRequest;
  jsonRequest["jsonrpc"] = "2.0";
  jsonRequest["method"] = method;
  if (params) {
    jsonRequest["params"] = *params;
  }

  if (!isNotification) {
    // Handling method call
    int requestId = GetNextRequestId();
    jsonRequest["id"] = requestId;

    std::promise<Json> responsePromise;
    auto futureResponse = responsePromise.get_future();

    {
      std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
      pendingRequests_[requestId] = std::move(responsePromise);
    }
    expectedResponses_++;

    transport_->SendRequest(jsonRequest.dump());

    // Block until the promise is fulfilled and return the response
    futureResponse.wait();
    auto response = futureResponse.get();

    {
      std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
      pendingRequests_.erase(requestId);
    }

    return response;
  } else {
    // Handling notification
    transport_->SendRequest(jsonRequest.dump());
    return std::nullopt;
  }
}

std::vector<Client::Response> Client::SendBatchRequest(
    const std::vector<Request> &requests) {
  Json jsonBatchRequest = Json::array();
  std::vector<int> ids;
  bool hasMethodCall = false;

  for (const auto &request : requests) {
    const std::string &method = std::get<0>(request);
    const std::optional<Json> &params = std::get<1>(request);
    bool isNotification = std::get<2>(request);

    Json jsonRequest;
    jsonRequest["jsonrpc"] = "2.0";
    jsonRequest["method"] = method;
    if (params) {
      jsonRequest["params"] = *params;
    }

    if (!isNotification) {
      int requestId = GetNextRequestId();
      jsonRequest["id"] = requestId;
      ids.push_back(requestId);

      std::promise<Json> responsePromise;
      {
        std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
        pendingRequests_[requestId] = std::move(responsePromise);
      }

      hasMethodCall = true;
    }

    jsonBatchRequest.push_back(jsonRequest);
  }

  if (hasMethodCall) {
    expectedResponses_++;
  }

  transport_->SendRequest(jsonBatchRequest.dump());

  std::vector<Response> responses;
  for (int id : ids) {
    auto future = pendingRequests_[id].get_future();
    future.wait();
    auto response = future.get();

    {
      std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
      pendingRequests_.erase(id);
    }

    responses.push_back(response);
  }

  return responses;
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

  if (jsonResponse.is_array()) {
    for (const auto &resp : jsonResponse) {
      if (ValidateResponse(resp)) {
        int requestId = resp["id"].get<int>();

        std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
        auto it = pendingRequests_.find(requestId);
        if (it != pendingRequests_.end()) {
          it->second.set_value(resp);
          pendingRequests_.erase(it);
        } else {
          spdlog::error(
              "Received response for unknown request ID: {}", requestId);
          throw std::runtime_error(
              "Received response for unknown request ID: " +
              std::to_string(requestId));
        }
      } else {
        spdlog::error("Invalid JSON-RPC response: {}", resp.dump());
        throw std::runtime_error("Invalid JSON-RPC response: " + resp.dump());
      }
    }
  } else {
    if (ValidateResponse(jsonResponse)) {
      int requestId = jsonResponse["id"].get<int>();

      std::lock_guard<std::mutex> lock(pendingRequestsMutex_);
      auto it = pendingRequests_.find(requestId);
      if (it != pendingRequests_.end()) {
        it->second.set_value(jsonResponse);
        pendingRequests_.erase(it);
      } else {
        spdlog::error(
            "Received response for unknown request ID: {}", requestId);
        throw std::runtime_error("Received response for unknown request ID: " +
                                 std::to_string(requestId));
      }
    } else {
      spdlog::error("Invalid JSON-RPC response: {}", jsonResponse.dump());
      throw std::runtime_error(
          "Invalid JSON-RPC response: " + jsonResponse.dump());
    }
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
