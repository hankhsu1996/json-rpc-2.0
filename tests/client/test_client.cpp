#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/client/client.hpp"
#include "jsonrpc/core/client_transport.hpp"

using namespace jsonrpc;

class MockClientTransport : public ClientTransport {
public:
  MockClientTransport() : methodCallCalled(false), notificationCalled(false) {
  }

  Json SendMethodCall(const Json &request) override {
    methodCallCalled = true;
    lastMethodCallRequest = request;
    return {};
  }

  void SendNotification(const Json &request) override {
    notificationCalled = true;
    lastNotificationRequest = request;
  }

  bool methodCallCalled;
  bool notificationCalled;
  Json lastMethodCallRequest;
  Json lastNotificationRequest;
};

TEST_CASE("Client sends method call", "[Client]") {
  using namespace jsonrpc;

  // Create a mock transport
  auto mockTransport = std::make_unique<MockClientTransport>();
  MockClientTransport *transportPtr = mockTransport.get();

  // Set up the Client
  Client client(std::move(mockTransport));

  // Expected request
  Json params = {{"param1", 1}, {"param2", 2}};
  Json expectedRequest = {{"jsonrpc", "2.0"}, {"method", "testMethod"},
      {"params", params}, {"id", 1}};

  // Call the method
  client.SendMethodCall("testMethod", params, 1);

  // Verify the mock transport was called correctly
  REQUIRE(transportPtr->methodCallCalled);
  REQUIRE(transportPtr->lastMethodCallRequest == expectedRequest);
}

TEST_CASE("Client sends notification", "[Client]") {
  using namespace jsonrpc;

  // Create a mock transport
  auto mockTransport = std::make_unique<MockClientTransport>();
  MockClientTransport *transportPtr = mockTransport.get();

  // Set up the Client
  Client client(std::move(mockTransport));

  // Expected notification
  Json params = {{"param1", 1}, {"param2", 2}};
  Json expectedNotification = {
      {"jsonrpc", "2.0"}, {"method", "notifyMethod"}, {"params", params}};

  // Call the method
  client.SendNotification("notifyMethod", params);

  // Verify the mock transport was called correctly
  REQUIRE(transportPtr->notificationCalled);
  REQUIRE(transportPtr->lastNotificationRequest == expectedNotification);
}
