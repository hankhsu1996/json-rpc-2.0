#include <jsonrpc/server/server.hpp>
#include <jsonrpc/transport/framed_socket_transport.hpp>
#include <spdlog/spdlog.h>

#include "../utils.hpp"

using jsonrpc::server::Server;
using jsonrpc::transport::FramedSocketTransport;

auto main() -> int {
  try {
    SetupLogger();

    auto transport =
        std::make_unique<FramedSocketTransport>("localhost", 2087, false);
    Server server(std::move(transport));

    RegisterLSPHandlers(server);

    spdlog::info("Starting LSP server...");
    server.Start();

  } catch (const std::exception& ex) {
    spdlog::error("Exception: {}", ex.what());
    return 1;
  }

  return 0;
}
