#include "jsonrpc/server/server.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc::server {

Server::Server(std::unique_ptr<transport::Transport> transport)
    : transport_(std::move(transport)) {
  dispatcher_ = std::make_unique<Dispatcher>();
  spdlog::info("Server initialized with transport");
}

void Server::Start() {
  spdlog::info("Server starting");
  running_.store(true);
  Listen();
}

void Server::Stop() {
  spdlog::info("Server stopping");
  running_.store(false);
}

auto Server::IsRunning() const -> bool {
  return running_.load();
}

void Server::Listen() {
  if (!dispatcher_) {
    spdlog::error("Dispatcher is not set.");
    return;
  }
  if (!transport_) {
    spdlog::error("Transport is not set.");
    return;
  }

  while (IsRunning()) {
    std::string request = transport_->ReceiveMessage();
    if (request.empty()) {
      continue;
    }
    std::optional<std::string> response = dispatcher_->DispatchRequest(request);
    if (response.has_value()) {
      transport_->SendMessage(response.value());
    }
  }
}

void Server::RegisterMethodCall(
    const std::string &method, const MethodCallHandler &handler) {
  dispatcher_->RegisterMethodCall(method, handler);
}

void Server::RegisterNotification(
    const std::string &method, const NotificationHandler &handler) {
  dispatcher_->RegisterNotification(method, handler);
}

}  // namespace jsonrpc::server
