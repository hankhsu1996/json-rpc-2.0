#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include "jsonrpc/server/server.hpp"
#include "jsonrpc/transport/stdio_transport.hpp"

#include "../common/mock_transport.hpp"

using namespace jsonrpc::server;
using namespace jsonrpc::transport;
using Json = nlohmann::json;

TEST_CASE("Server initializes correctly", "[Server]") {
  Server server(std::make_unique<MockTransport>());
}

TEST_CASE("Server stops and exits running state", "[Server]") {
  auto mock_transport = std::make_unique<MockTransport>();
  Server server(std::move(mock_transport));

  std::thread server_thread([&server]() { server.Start(); });

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  server.Stop();

  if (server_thread.joinable()) {
    server_thread.join();
  }

  REQUIRE(server.IsRunning() == false);
}
