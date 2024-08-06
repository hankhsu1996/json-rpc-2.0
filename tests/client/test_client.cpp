#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "jsonrpc/client/client.hpp"

#include "../common/mock_transport.hpp"

using namespace jsonrpc::client;
using namespace jsonrpc::transport;

TEST_CASE("Client starts and stops correctly", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  Client client(std::move(transport));

  client.Start();
  REQUIRE(client.IsRunning() == true);
  REQUIRE(client.HasPendingRequests() == false);

  client.Stop();
  REQUIRE(client.IsRunning() == false);
}

TEST_CASE("Client handles responses correctly", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  transport->SetResponse(R"({"jsonrpc":"2.0","result":"success","id":0})");

  Client client(std::move(transport));
  client.Start();

  REQUIRE(client.HasPendingRequests() == false);
  auto response = client.SendMethodCall("test_method");
  REQUIRE(client.HasPendingRequests() == false);

  REQUIRE(response["result"] == "success");

  client.Stop();
}

TEST_CASE(
    "Client sends notification without expecting a response", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transportPtr = transport.get();

  Client client(std::move(transport));
  client.Start();

  client.SendNotification(
      "notify_event", nlohmann::json({{"param1", "value1"}}));

  REQUIRE(client.HasPendingRequests() == false);
  REQUIRE(transportPtr->sentRequests.size() == 1);
  REQUIRE(
      transportPtr->sentRequests[0].find("notify_event") != std::string::npos);

  client.Stop();
}

TEST_CASE("Client handles valid JSON-RPC response", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transportPtr = transport.get();

  transportPtr->SetResponse(
      R"({"jsonrpc":"2.0","result":{"data":"success"},"id":0})");

  Client client(std::move(transport));
  client.Start();

  REQUIRE(client.HasPendingRequests() == false);
  auto response = client.SendMethodCall("getData");
  REQUIRE(client.HasPendingRequests() == false);

  REQUIRE(response.contains("result"));
  REQUIRE(response["result"].contains("data"));
  REQUIRE(response["result"]["data"] == "success");

  client.Stop();
}

TEST_CASE("Client handles in-order responses correctly", "[Client][InOrder]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transportPtr = transport.get();

  for (int i = 0; i < 10; ++i) {
    std::string response = fmt::format(
        R"({{"jsonrpc":"2.0","result":{{"data":"success_{}"}},"id":{}}})", i,
        i);
    transportPtr->SetResponse(response);
  }

  Client client(std::move(transport));
  client.Start();

  for (int i = 0; i < 10; ++i) {
    REQUIRE(client.HasPendingRequests() == false);
    auto response = client.SendMethodCall("getData");
    REQUIRE(client.HasPendingRequests() == false);
    REQUIRE(response["result"]["data"] == "success_" + std::to_string(i));
  }

  client.Stop();
}

TEST_CASE("Client handles async method calls correctly", "[Client][Async]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transportPtr = transport.get();

  transportPtr->SetResponse(
      R"({"jsonrpc":"2.0","result":"async_success","id":0})");

  Client client(std::move(transport));
  client.Start();

  auto futureResponse = client.SendMethodCallAsync("async_test_method");

  REQUIRE(futureResponse.wait_for(std::chrono::seconds(1)) ==
          std::future_status::ready);

  nlohmann::json response = futureResponse.get();

  REQUIRE(client.HasPendingRequests() == false);
  REQUIRE(response["result"] == "async_success");

  client.Stop();
}

TEST_CASE("Client handles multiple async method calls concurrently",
    "[Client][Async]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transportPtr = transport.get();

  for (int i = 0; i < 5; ++i) {
    transportPtr->SetResponse(fmt::format(
        R"({{"jsonrpc":"2.0","result":"success_{}","id":{}}})", i, i));
  }

  Client client(std::move(transport));
  client.Start();

  std::vector<std::future<nlohmann::json>> futures;

  for (int i = 0; i < 5; ++i) {
    futures.push_back(
        client.SendMethodCallAsync(fmt::format("async_test_method_{}", i)));
  }

  for (int i = 0; i < 5; ++i) {
    REQUIRE(futures[i].wait_for(std::chrono::seconds(1)) ==
            std::future_status::ready);
    nlohmann::json response = futures[i].get();
    REQUIRE(response["result"] == fmt::format("success_{}", i));
  }

  REQUIRE(client.HasPendingRequests() == false);

  client.Stop();
}
