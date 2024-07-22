#include <memory>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <spdlog/spdlog.h>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/server/server.hpp"
#include "json_rpc/transports/stdio_server_transport.hpp"

using namespace json_rpc;

class MockServerTransport : public ServerTransport {
public:
  MockServerTransport() : ServerTransport() {
  }

protected:
  void Listen() override {
    spdlog::info("MockServerTransport listening");
    while (IsRunning()) {
      // Simulate some work
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    spdlog::info("MockServerTransport stopped listening");
  }
};

TEST_CASE("Server initialization", "[server]") {
  auto transport = std::make_unique<MockServerTransport>();
  Server server(std::move(transport));
  REQUIRE_NOTHROW(server);
}

TEST_CASE("Server start and stop", "[server]") {
  auto transport = std::make_unique<MockServerTransport>();
  Server server(std::move(transport));

  REQUIRE_FALSE(server.IsRunning());

  // Start the server in a separate thread
  std::thread serverThread([&server]() { REQUIRE_NOTHROW(server.Start()); });

  // Simulate some runtime
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(server.IsRunning());

  REQUIRE_NOTHROW(server.Stop());
  if (serverThread.joinable()) {
    serverThread.join();
  }

  REQUIRE_FALSE(server.IsRunning());
}

TEST_CASE("Server method registration", "[server]") {
  auto transport = std::make_unique<MockServerTransport>();
  Server server(std::move(transport));

  MethodCallHandler methodHandler =
      [](const std::optional<Json> &params) -> Json {
    return {{"result", "testMethod"}};
  };

  NotificationHandler notificationHandler =
      [](const std::optional<Json> &params) {};

  REQUIRE_NOTHROW(server.RegisterMethodCall("testMethod", methodHandler));
  REQUIRE_NOTHROW(
      server.RegisterNotification("testNotification", notificationHandler));
}
