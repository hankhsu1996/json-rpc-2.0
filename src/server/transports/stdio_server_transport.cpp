#include "jsonrpc/server/transports/stdio_server_transport.hpp"

#include <spdlog/spdlog.h>

namespace jsonrpc {
namespace server {
namespace transports {

void StdioServerTransport::Listen() {
  spdlog::info("StdioServerTransport listening");
  std::string line;

  if (!dispatcher_) {
    spdlog::error("Dispatcher is not set.");
    return;
  }

  while (IsRunning()) {
    if (std::getline(std::cin, line)) {
      std::optional<std::string> response = dispatcher_->DispatchRequest(line);
      if (response.has_value()) {
        spdlog::debug(
            "StdioServerTransport dispatching response: {}", response.value());
        std::cout << response.value() << std::endl;
      }
    } else {
      break; // Exit loop if std::getline fails (e.g., EOF)
    }
  }
}

} // namespace transports
} // namespace server
} // namespace jsonrpc
