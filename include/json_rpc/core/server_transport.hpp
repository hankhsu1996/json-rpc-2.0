#pragma once

#include <atomic>
#include <memory>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

/**
 * @brief Abstract base class for server transport mechanisms.
 *
 * The ServerTransport class provides an interface for various transport
 * mechanisms that a JSON-RPC server can use, such as stdio and HTTP. Concrete
 * implementations must implement the Listen method to handle incoming messages.
 */
class ServerTransport {
public:
  /**
   * @brief Constructs a ServerTransport object.
   */
  ServerTransport() : dispatcher_(nullptr), running_(false) {
  }

  /**
   * @brief Virtual destructor for ServerTransport.
   */
  virtual ~ServerTransport() = default;

  /**
   * @brief Starts the transport.
   *
   * This method sets the running state to true and calls the Listen method.
   */
  virtual void Start() {
    running_.store(true, std::memory_order_release);
    Listen();
  }

  /**
   * @brief Stops the transport.
   *
   * This method sets the running state to false.
   */
  virtual void Stop() {
    running_.store(false, std::memory_order_release);
  }

  /**
   * @brief Sets the dispatcher for the transport.
   *
   * @param dispatcher A pointer to the Dispatcher object.
   */
  void SetDispatcher(Dispatcher *dispatcher) {
    dispatcher_ = dispatcher;
  }

  /**
   * @brief Checks if the transport is running.
   *
   * @return True if the transport is running, false otherwise.
   */
  bool IsRunning() const {
    return running_.load(std::memory_order_acquire);
  }

protected:
  /**
   * @brief Listens for incoming messages.
   *
   * This is a pure virtual method that must be implemented by concrete
   * transport classes.
   */
  virtual void Listen() = 0;

  /// @brief Atomic flag indicating if the transport is running.
  std::atomic<bool> running_;

  /// @brief Pointer to the Dispatcher object.
  Dispatcher *dispatcher_;
};

} // namespace json_rpc
