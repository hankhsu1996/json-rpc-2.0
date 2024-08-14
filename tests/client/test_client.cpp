#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "../common/mock_transport.hpp"
#include "jsonrpc/client/client.hpp"

TEST_CASE("Client starts and stops correctly", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  jsonrpc::client::Client client(std::move(transport));

  client.Start();
  REQUIRE(client.IsRunning() == true);
  REQUIRE(client.HasPendingRequests() == false);

  client.Stop();
  REQUIRE(client.IsRunning() == false);
}

TEST_CASE("Client handles responses correctly", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  transport->SetResponse(R"({"jsonrpc":"2.0","result":"success","id":0})");

  jsonrpc::client::Client client(std::move(transport));
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
  MockTransport *transport_ptr = transport.get();

  jsonrpc::client::Client client(std::move(transport));
  client.Start();

  client.SendNotification(
      "notify_event", nlohmann::json({{"param1", "value1"}}));

  REQUIRE(client.HasPendingRequests() == false);
  REQUIRE(transport_ptr->sent_requests.size() == 1);
  REQUIRE(
      transport_ptr->sent_requests[0].find("notify_event") !=
      std::string::npos);

  client.Stop();
}

TEST_CASE("Client handles valid JSON-RPC response", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transport_ptr = transport.get();

  transport_ptr->SetResponse(
      R"({"jsonrpc":"2.0","result":{"data":"success"},"id":0})");

  jsonrpc::client::Client client(std::move(transport));
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
  MockTransport *transport_ptr = transport.get();

  const int num_requests = 10;
  for (int i = 0; i < num_requests; ++i) {
    std::string response = fmt::format(
        R"({{"jsonrpc":"2.0","result":{{"data":"success_{}"}},"id":{}}})", i,
        i);
    transport_ptr->SetResponse(response);
  }

  jsonrpc::client::Client client(std::move(transport));
  client.Start();

  for (int i = 0; i < num_requests; ++i) {
    REQUIRE(client.HasPendingRequests() == false);
    auto response = client.SendMethodCall("getData");
    REQUIRE(client.HasPendingRequests() == false);
    REQUIRE(response["result"]["data"] == "success_" + std::to_string(i));
  }

  client.Stop();
}

TEST_CASE("Client handles async method calls correctly", "[Client][Async]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transport_ptr = transport.get();

  transport_ptr->SetResponse(
      R"({"jsonrpc":"2.0","result":"async_success","id":0})");

  jsonrpc::client::Client client(std::move(transport));
  client.Start();

  auto future_response = client.SendMethodCallAsync("async_test_method");

  REQUIRE(
      future_response.wait_for(std::chrono::seconds(1)) ==
      std::future_status::ready);

  nlohmann::json response = future_response.get();

  REQUIRE(client.HasPendingRequests() == false);
  REQUIRE(response["result"] == "async_success");

  client.Stop();
}

TEST_CASE(
    "Client handles multiple async method calls concurrently",
    "[Client][Async]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transport_ptr = transport.get();

  const int num_requests = 5;
  for (int i = 0; i < num_requests; ++i) {
    transport_ptr->SetResponse(fmt::format(
        R"({{"jsonrpc":"2.0","result":"success_{}","id":{}}})", i, i));
  }

  jsonrpc::client::Client client(std::move(transport));
  client.Start();

  std::vector<std::future<nlohmann::json>> futures;

  for (int i = 0; i < num_requests; ++i) {
    futures.push_back(
        client.SendMethodCallAsync(fmt::format("async_test_method_{}", i)));
  }

  for (int i = 0; i < num_requests; ++i) {
    REQUIRE(
        futures[i].wait_for(std::chrono::seconds(1)) ==
        std::future_status::ready);
    nlohmann::json response = futures[i].get();
    REQUIRE(response["result"] == fmt::format("success_{}", i));
  }

  REQUIRE(client.HasPendingRequests() == false);

  client.Stop();
}
