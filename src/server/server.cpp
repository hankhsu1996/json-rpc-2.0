#include "jsonrpc/server/server.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace server {

Server::Server(std::unique_ptr<transports::ServerTransport> transport)
    : transport_(std::move(transport)) {
  dispatcher_ = std::make_shared<Dispatcher>();
  transport_->SetDispatcher(dispatcher_);
  spdlog::info("Server initialized with transport");
}

void Server::Start() {
  spdlog::info("Server starting");
  transport_->Start();
}

void Server::Stop() {
  spdlog::info("Server stopping");
  transport_->Stop();
}

bool Server::IsRunning() const {
  return transport_->IsRunning();
}

void Server::RegisterMethodCall(
    const std::string &method, const MethodCallHandler &handler) {
  dispatcher_->RegisterMethodCall(method, handler);
}

void Server::RegisterNotification(
    const std::string &method, const NotificationHandler &handler) {
  dispatcher_->RegisterNotification(method, handler);
}

} // namespace server
} // namespace jsonrpc
