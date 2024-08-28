#include <string>

#include <jsonrpc/server/server.hpp>
#include <jsonrpc/transport/framed_pipe_transport.hpp>
#include <spdlog/spdlog.h>

#include "../utils.hpp"

using jsonrpc::server::Server;
using jsonrpc::transport::FramedPipeTransport;

auto main(int argc, char* argv[]) -> int {
  try {
    std::vector<std::string> args(argv, argv + argc);
    std::string pipe_name = ParsePipeArguments(args);
    SetupLogger();

    // The 'false' argument indicates that the transport is acting as a client.
    // In this setup, VS Code creates and owns the pipe, and the LSP server
    // (this process) connects to the pipe as a client.
    auto transport = std::make_unique<FramedPipeTransport>(pipe_name, false);
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
