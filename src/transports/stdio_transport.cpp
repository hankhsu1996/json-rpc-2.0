#include "json_rpc/transports/stdio_transport.h"

namespace json_rpc {

void StdioServerTransport::Listen() {
  std::string line;
  while (running_) {
    if (std::getline(std::cin, line)) {
      std::optional<std::string> response = dispatcher_->Dispatch(line);
      if (response.has_value()) {
        std::cout << response.value() << std::endl;
      }
    } else {
      break; // Exit loop if std::getline fails (e.g., EOF)
    }
  }
}

Json StdioClientTransport::SendMethodCall(const Json &request) {
  std::cout << request.dump() << std::endl;
  std::string line;
  std::getline(std::cin, line);
  return Json::parse(line);
}

void StdioClientTransport::SendNotification(const Json &notification) {
  std::cout << notification.dump() << std::endl;
}

} // namespace json_rpc
