#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "json_rpc/core/request.hpp"
#include "json_rpc/core/response.hpp"
#include "json_rpc/core/types.hpp"

#include "BS_thread_pool.hpp"

namespace json_rpc {

/**
 * @brief A class responsible for dispatching JSON-RPC requests to the
 * appropriate handlers.
 *
 * The Dispatcher class enables the registration and execution of method call
 * and notification handlers for JSON-RPC requests. Handlers can be registered
 * using std::function, allowing for flexible and modern C++ usage. The
 * Dispatcher can operate in single-threaded or multi-threaded mode, providing
 * scalability for handling batch requests.
 */
class Dispatcher {
public:
  /**
   * @brief Constructs a Dispatcher object.
   *
   * @param enableMultithreading Enable multi-threading support.
   * @param numThreads Number of threads to use if multi-threading is enabled.
   */
  Dispatcher(bool enableMultithreading = true,
      size_t numThreads = std::thread::hardware_concurrency())
      : enableMultithreading_(enableMultithreading),
        thread_pool_(enableMultithreading ? numThreads : 0) {
  }

  /**
   * @brief Destructor for Dispatcher.
   */
  virtual ~Dispatcher() = default;

  /**
   * @brief Dispatches an RPC request to the appropriate handler.
   *
   * @param request The JSON-RPC request as a string.
   * @return The response from the handler as a JSON string, or std::nullopt if
   * no response is needed.
   */
  std::optional<std::string> DispatchRequest(const std::string &request);

  /**
   * @brief Registers a method call handler for a specific method.
   *
   * @param method The name of the method.
   * @param handler The handler function to be called for this method.
   */
  void RegisterMethodCall(
      const std::string &method, const MethodCallHandler &handler);

  /**
   * @brief Registers a notification handler for a specific method.
   *
   * @param method The name of the method.
   * @param handler The handler function to be called for this notification.
   */
  void RegisterNotification(
      const std::string &method, const NotificationHandler &handler);

private:
  /// @brief A map of method names to method call handlers.
  std::unordered_map<std::string, Handler> handlers_;

  /// @brief Flag to enable multi-threading support.
  bool enableMultithreading_;

  /// @brief Thread pool for multi-threading.
  BS::thread_pool thread_pool_;

  /// @brief Parses and validates the JSON request string.
  std::optional<Json> ParseAndValidateJson(const std::string &requestStr);

  /**
   * @brief Dispatches a single request to the appropriate handler and returns a
   * JSON string.
   */
  std::optional<std::string> DispatchSingleRequest(const Json &requestJson);

  /**
   * @brief Internal method to dispatch a single request to the appropriate
   * handler and returns a JSON object.
   */
  std::optional<Json> DispatchSingleRequestInner(const Json &requestJson);

  /**
   * @brief Dispatches a batch request to the appropriate handlers and returns a
   * JSON string.
   */
  std::optional<std::string> DispatchBatchRequest(const Json &requestJson);

  /**
   * @brief Internal method to dispatch a batch request to the appropriate
   * handlers and returns a vector of JSON objects.
   */
  std::vector<Json> DispatchBatchRequestInner(const Json &requestJson);

  /**
   * @brief Validates the request JSON object.
   */
  std::optional<Response> ValidateRequest(const Json &requestJson);

  /**
   * @brief Finds the handler for the specified method.
   */
  std::optional<Handler> FindHandler(
      const std::unordered_map<std::string, Handler> &handlers,
      const std::string &method);

  /**
   * @brief Handles the request (method call or notification) using the
   * appropriate handler.
   */
  std::optional<Json> HandleRequest(
      const Request &request, const Handler &handler);

  /**
   * @brief Handles a method call request.
   */
  Response HandleMethodCall(
      const Request &request, const MethodCallHandler &handler);

  /**
   * @brief Handles a notification request.
   */
  void HandleNotification(
      const Request &request, const NotificationHandler &handler);
};

} // namespace json_rpc
