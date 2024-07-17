#include "json_rpc/transports/stdio_transport.h"

namespace json_rpc {

void StdioTransport::Listen() {
  std::string line;
  while (std::getline(std::cin, line)) {
    std::optional<std::string> response = dispatcher_->Dispatch(line);
    if (response.has_value()) {
      std::cout << response.value() << std::endl;
    }
  }
}

Json StdioTransport::SendMethodCall(const Json &request) {
  std::cout << request.dump() << std::endl;
  std::string line;
  std::getline(std::cin, line);
  return Json::parse(line);
}

void StdioTransport::SendNotification(const Json &notification) {
  std::cout << notification.dump() << std::endl;
}

} // namespace json_rpc
