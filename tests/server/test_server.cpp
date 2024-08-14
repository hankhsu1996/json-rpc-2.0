#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include "../common/mock_transport.hpp"
#include "jsonrpc/server/server.hpp"

using Json = nlohmann::json;

TEST_CASE("Server initializes correctly", "[Server]") {
  jsonrpc::server::Server server(std::make_unique<MockTransport>());
}

TEST_CASE("Server stops and exits running state", "[Server]") {
  auto mock_transport = std::make_unique<MockTransport>();
  jsonrpc::server::Server server(std::move(mock_transport));

  std::thread server_thread([&server]() { server.Start(); });

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  server.Stop();

  if (server_thread.joinable()) {
    server_thread.join();
  }

  REQUIRE(server.IsRunning() == false);
}
