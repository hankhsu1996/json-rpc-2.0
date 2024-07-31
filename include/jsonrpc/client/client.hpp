#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "jsonrpc/core/client_transport.hpp"
#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

/**
 * @brief A JSON-RPC client for sending requests and receiving responses.
 */
class Client {
public:
  /**
   * @brief A JSON-RPC request.
   *
   * The request is a tuple containing the method name, parameters, and a flag
   * indicating if the request is a notification.
   */
  using Request = std::tuple<std::string, std::optional<Json>, bool>;

  /**
   * @brief A JSON-RPC response.
   *
   * The response is an optional JSON object. If the response is a method call,
   * the JSON object contains the response data. If the response is a
   * notification, the JSON object is `std::nullopt`.
   */
  using Response = std::optional<Json>;

  /**
   * @brief Constructs a JSON-RPC client with a specified transport layer.
   * @param transport A unique pointer to a transport layer.
   */
  Client(std::unique_ptr<ClientTransport> transport);
  ~Client() = default;

  /**
   * @brief Starts the JSON-RPC client listener thread.
   */
  void Start();

  /**
   * @brief Stops the JSON-RPC client listener thread.
   */
  void Stop();

  /**
   * @brief Sends a single JSON-RPC request.
   * @param request The JSON-RPC request to send.
   * @return The JSON-RPC response if it is a method call, otherwise
   * std::nullopt.
   */
  Response SendRequest(const Request &request);

  /**
   * @brief Sends a batch of JSON-RPC requests.
   * @param requests A vector of JSON-RPC requests to send.
   * @return A vector of JSON-RPC responses. Each response corresponds to a
   * request.
   */
  std::vector<Response> SendBatchRequest(const std::vector<Request> &requests);

private:
  /**
   * @brief Listener thread function for receiving responses from the transport
   * layer.
   */
  void Listener();

  /**
   * @brief Generates the next unique request ID.
   * @return The next unique request ID.
   */
  int GetNextRequestId();

  /**
   * @brief Handles a JSON-RPC response received from the transport layer.
   * @param response The JSON-RPC response as a string.
   */
  void HandleResponse(const std::string &response);

  /**
   * @brief Validates a JSON-RPC response.
   * @param response The JSON-RPC response to validate.
   * @return True if the response is valid, false otherwise.
   */
  bool ValidateResponse(const Json &response);

  /// Transport layer for communication.
  std::unique_ptr<ClientTransport> transport_;

  /// Counter for generating unique request IDs.
  std::atomic<int> requestCounter_{0};

  /// Counter for tracking expected responses.
  std::atomic<int> expectedResponses_{0};

  /// Map of pending requests and their associated promises.
  std::unordered_map<int, std::promise<Json>> pendingRequests_;

  /// Mutex to protect access to the pending requests map.
  std::mutex pendingRequestsMutex_;

  /// Listener thread for receiving responses.
  std::thread listenerThread_;

  /// Flag to control the running state of the listener thread.
  std::atomic<bool> running_{true};
};

} // namespace jsonrpc
