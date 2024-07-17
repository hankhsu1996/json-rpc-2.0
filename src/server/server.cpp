#include "json_rpc/server/server.h"

namespace json_rpc {

Server::Server(std::unique_ptr<ServerTransport> transport)
    : transport_(std::move(transport)) {
  dispatcher_ = std::make_unique<Dispatcher>();
  transport_->SetDispatcher(dispatcher_.get());
}

void Server::Start() {
  transport_->Listen();
}

void Server::RegisterMethodCall(
    const std::string &method, const MethodCallHandler &handler) {
  dispatcher_->RegisterMethodCall(method, handler);
}

void Server::RegisterNotification(
    const std::string &method, const NotificationHandler &handler) {
  dispatcher_->RegisterNotification(method, handler);
}

} // namespace json_rpc
