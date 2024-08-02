#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <BS_thread_pool.hpp>

#include "jsonrpc/core/request.hpp"
#include "jsonrpc/core/response.hpp"
#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

/**
 * @brief Interface for JSON-RPC request dispatchers.
 *
 * IDispatcher defines the contract for classes that process JSON-RPC requests.
 */
class IDispatcher {
public:
  virtual ~IDispatcher() = default;

  /**
   * @brief Processes a JSON-RPC request.
   *
   * @param request The JSON-RPC request as a string.
   * @return The response as a JSON string, or std::nullopt if no response is
   * needed.
   */
  virtual std::optional<std::string> DispatchRequest(
      const std::string &request) = 0;
};

/**
 * @brief Dispatcher for JSON-RPC requests.
 *
 * Dispatcher manages the registration and execution of method call and
 * notification handlers for JSON-RPC requests. It can operate in
 * single-threaded or multi-threaded mode.
 */
class Dispatcher : public IDispatcher {
public:
  /**
   * @brief Constructs a Dispatcher.
   *
   * @param enableMultithreading Enable multi-threading support.
   * @param numThreads Number of threads to use if multi-threading is enabled.
   */
  Dispatcher(bool enableMultithreading = true,
      size_t numThreads = std::thread::hardware_concurrency());

  /// @brief Destructor.
  virtual ~Dispatcher() = default;

  /**
   * @brief Processes a JSON-RPC request.
   *
   * Dispatches the request to the appropriate handler.
   *
   * @param request The JSON-RPC request as a string.
   * @return The response from the handler as a JSON string, or std::nullopt if
   * no response is needed.
   */
  std::optional<std::string> DispatchRequest(
      const std::string &request) override;

  /**
   * @brief Registers a method call handler.
   *
   * @param method The name of the RPC method.
   * @param handler The handler function for this method.
   */
  void RegisterMethodCall(
      const std::string &method, const MethodCallHandler &handler);

  /**
   * @brief Registers a notification handler.
   *
   * @param method The name of the RPC notification.
   * @param handler The handler function for this notification.
   */
  void RegisterNotification(
      const std::string &method, const NotificationHandler &handler);

private:
  /**
   * @brief Parses and validates the JSON request string.
   *
   * Attempts to parse the input string as JSON. If parsing fails, logs an error
   * and returns std::nullopt.
   *
   * @param requestStr The JSON-RPC request as a string.
   * @return The parsed JSON object, or std::nullopt if parsing failed.
   */
  std::optional<Json> ParseAndValidateJson(const std::string &requestStr);

  /**
   * @brief Dispatches a single request to the appropriate handler and returns a
   * JSON string.
   *
   * This method handles single JSON-RPC requests, delegating to the appropriate
   * handler or generating an error response.
   *
   * @param requestJson The parsed JSON request.
   * @return The response as a JSON string, or std::nullopt if no response is
   * needed.
   */
  std::optional<std::string> DispatchSingleRequest(const Json &requestJson);

  /**
   * @brief Internal method to dispatch a single request to the appropriate
   * handler and returns a JSON object.
   *
   * Validates the request, finds the appropriate handler, and processes the
   * request. If an error occurs, a JSON error response is generated.
   *
   * @param requestJson The parsed JSON request.
   * @return The response as a JSON object, or std::nullopt if no response is
   * needed.
   */
  std::optional<Json> DispatchSingleRequestInner(const Json &requestJson);

  /**
   * @brief Dispatches a batch request to the appropriate handlers and returns a
   * JSON string.
   *
   * Handles a batch of JSON-RPC requests, processing each one concurrently if
   * multithreading is enabled.
   *
   * @param requestJson The parsed JSON batch request.
   * @return The batch response as a JSON string, or std::nullopt if no
   * responses are needed.
   */
  std::optional<std::string> DispatchBatchRequest(const Json &requestJson);

  /**
   * @brief Internal method to dispatch a batch request to the appropriate
   * handlers and returns a vector of JSON objects.
   *
   * Processes each request in the batch, potentially using multithreading to
   * handle multiple requests concurrently.
   *
   * @param requestJson The parsed JSON batch request.
   * @return A vector of JSON objects representing the responses.
   */
  std::vector<Json> DispatchBatchRequestInner(const Json &requestJson);

  /**
   * @brief Validates the request JSON object.
   *
   * Checks the structure and required fields of a JSON-RPC request. If
   * validation fails, returns an appropriate error response.
   *
   * @param requestJson The JSON-RPC request object.
   * @return An error response if validation fails, or std::nullopt if
   * validation succeeds.
   */
  std::optional<Response> ValidateRequest(const Json &requestJson);

  /**
   * @brief Finds the handler for the specified method.
   *
   * Looks up the handler for a given method name in the registered handlers
   * map.
   *
   * @param handlers The map of method names to handlers.
   * @param method The name of the method to find.
   * @return The handler for the method, or std::nullopt if not found.
   */
  std::optional<Handler> FindHandler(
      const std::unordered_map<std::string, Handler> &handlers,
      const std::string &method);

  /**
   * @brief Handles the request (method call or notification) using the
   * appropriate handler.
   *
   * Determines whether the request is a method call or notification, and
   * processes it accordingly using the provided handler.
   *
   * @param request The parsed JSON-RPC request.
   * @param handler The handler to process the request.
   * @return The response as a JSON object, or std::nullopt if no response is
   * needed.
   */
  std::optional<Json> HandleRequest(
      const Request &request, const Handler &handler);

  /**
   * @brief Handles a method call request.
   *
   * Executes the registered method call handler and returns the response.
   *
   * @param request The parsed JSON-RPC request.
   * @param handler The method call handler to execute.
   * @return The JSON-RPC response object.
   */
  Response HandleMethodCall(
      const Request &request, const MethodCallHandler &handler);

  /**
   * @brief Handles a notification request.
   *
   * Executes the registered notification handler. Notifications do not require
   * a response.
   *
   * @param request The parsed JSON-RPC request.
   * @param handler The notification handler to execute.
   */
  void HandleNotification(
      const Request &request, const NotificationHandler &handler);

  /// @brief A map of method names to method call handlers.
  std::unordered_map<std::string, Handler> handlers_;

  /// @brief Flag to enable multi-threading support.
  bool enableMultithreading_;

  /// @brief Thread pool for multi-threading.
  BS::thread_pool thread_pool_;
};

} // namespace jsonrpc
