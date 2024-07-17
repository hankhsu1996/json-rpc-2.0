#include "json_rpc/server/server.h"

namespace json_rpc {

Server::Server(std::unique_ptr<Transport> transport)
    : _transport(std::move(transport)) {
  _dispatcher = std::make_unique<Dispatcher>();
  _transport->set_dispatcher(_dispatcher.get());
}

void Server::start() {
  _transport->listen();
}

void Server::registerMethodCall(
    const std::string &method, const MethodCallHandler &handler) {
  _dispatcher->registerMethodCall(method, handler);
}

void Server::registerNotification(
    const std::string &method, const NotificationHandler &handler) {
  _dispatcher->registerNotification(method, handler);
}

} // namespace json_rpc
