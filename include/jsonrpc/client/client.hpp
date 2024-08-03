#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include "jsonrpc/client/request.hpp"
#include "jsonrpc/client/transports/client_transport.hpp"

namespace jsonrpc {
namespace client {

/// @brief A JSON-RPC client for sending requests and receiving responses.
class Client {
public:
  /**
   * @brief Constructs a JSON-RPC client with a specified transport layer.
   *
   * Initializes the client with the provided transport layer, which handles
   * the communication between the client and the server.
   *
   * @param transport A unique pointer to a transport layer used for
   * communication.
   */
  Client(std::unique_ptr<transports::ClientTransport> transport);

  /// @brief Destructor.
  ~Client() = default;

  /**
   * @brief Starts the JSON-RPC client listener thread.
   *
   * Initializes and starts a background thread that listens for responses from
   * the server. This must be called before sending any requests.
   *
   * @throws std::runtime_error If the listener thread fails to start.
   */
  void Start();

  /**
   * @brief Stops the JSON-RPC client listener thread.
   *
   * Stops the background listener thread and waits for it to join. This should
   * be called before the client is destroyed or when the client no longer needs
   * to listen for responses.
   */
  void Stop();

  /**
   * @brief Checks if the client listener is running.
   *
   * @return True if the listener thread is running, false otherwise.
   */
  bool isRunning() const;

  /**
   * @brief Sends a JSON-RPC method call and waits for the response.
   *
   * Sends a JSON-RPC method call to the server and waits for the corresponding
   * response. The response is returned as a JSON object.
   *
   * @param method The name of the method to call.
   * @param params Optional parameters to pass to the method.
   * @return The JSON response received from the server.
   *
   * @see Json for details about the JSON type used in the library.
   */
  nlohmann::json SendMethodCall(const std::string &method,
      std::optional<nlohmann::json> params = std::nullopt);

  /**
   * @brief Sends a JSON-RPC notification.
   *
   * Sends a JSON-RPC notification to the server. Unlike method calls,
   * notifications do not expect a response from the server.
   *
   * @param method The name of the method to notify.
   * @param params Optional parameters to pass to the method.
   *
   * @see Json for details about the JSON type used in the library.
   */
  void SendNotification(const std::string &method,
      std::optional<nlohmann::json> params = std::nullopt);

private:
  /// @brief Listener thread function for receiving responses from the transport
  /// layer.
  void Listener();

  /**
   * @brief Helper function to send a request and optionally wait for a
   * response.
   *
   * Depending on the type of request (method call or notification), this
   * function will either wait for a response or send the request without
   * expecting one.
   *
   * @param request The JSON-RPC request to be sent.
   * @return The JSON response received from the server.
   */
  nlohmann::json SendRequest(const ClientRequest &request);

  /**
   * @brief Generates the next unique request ID.
   *
   * @return An integer representing the next unique request ID.
   */
  int GetNextRequestId();

  /**
   * @brief Handles a JSON-RPC response received from the transport layer.
   *
   * Parses the response and resolves the associated promise with the response
   * data.
   *
   * @param response The JSON-RPC response as a string.
   *
   * @throws std::runtime_error If the response is invalid or cannot be matched
   * to a request.
   */
  void HandleResponse(const std::string &response);

  /**
   * @brief Validates a JSON-RPC response.
   *
   * Ensures that the response conforms to the JSON-RPC 2.0 specification.
   *
   * @param response The JSON-RPC response as a JSON object.
   * @return True if the response is valid, false otherwise.
   */
  bool ValidateResponse(const nlohmann::json &response);

  /// @brief Transport layer for communication.
  std::unique_ptr<transports::ClientTransport> transport_;

  /// @brief Counter for generating unique request IDs.
  std::atomic<int> requestCounter_{0};

  /// @brief Counter for tracking expected responses.
  std::atomic<int> expectedResponses_{0};

  /// @brief Map of pending requests and their associated promises.
  std::unordered_map<int, std::promise<nlohmann::json>> pendingRequests_;

  /// @brief Mutex to protect access to the pending requests map.
  std::mutex pendingRequestsMutex_;

  /// @brief Listener thread for receiving responses.
  std::thread listenerThread_;

  /// @brief Flag to control the running state of the listener thread.
  std::atomic<bool> running_{true};
};

} // namespace client
} // namespace jsonrpc
