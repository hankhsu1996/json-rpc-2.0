#pragma once

#include <atomic>
#include <memory>

#include "json_rpc/core/dispatcher.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class ServerTransport {
public:
  ServerTransport() : dispatcher_(nullptr), running_(false) {
  }

  virtual ~ServerTransport() = default;

  // Start the transport.
  virtual void Start() {
    running_.store(true, std::memory_order_release);
    Listen();
  }

  // Stop the transport.
  virtual void Stop() {
    running_.store(false, std::memory_order_release);
  }

  // Set the dispatcher.
  void SetDispatcher(Dispatcher *dispatcher) {
    dispatcher_ = dispatcher;
  }

  // Check if the transport is running.
  bool IsRunning() const {
    return running_.load(std::memory_order_acquire);
  }

protected:
  // Listen for incoming messages.
  virtual void Listen() = 0;

  std::atomic<bool> running_;
  Dispatcher *dispatcher_;
};

} // namespace json_rpc
