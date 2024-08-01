#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "jsonrpc/client/client.hpp"
#include "jsonrpc/core/client_transport.hpp"

using namespace jsonrpc;

class MockTransport : public ClientTransport {
public:
  std::vector<std::string> sentRequests;
  std::queue<std::string> responses;

  void SendRequest(const std::string &request) override {
    sentRequests.push_back(request);
  }

  std::string ReadResponse() override {
    if (responses.empty()) {
      return ""; // Simulate no response or timeout
    }
    std::string response = responses.front();
    responses.pop();
    return response;
  }

  void SetResponse(const std::string &response) {
    responses.push(response);
  }
};

TEST_CASE("Client starts and stops correctly", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  Client client(std::move(transport));

  client.Start();
  REQUIRE(client.isRunning() == true);

  client.Stop();
  REQUIRE(client.isRunning() == false);
}

TEST_CASE("Client handles responses correctly", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  transport->SetResponse(R"({"jsonrpc":"2.0","result":"success","id":0})");

  Client client(std::move(transport));
  client.Start();

  auto response = client.SendMethodCall("test_method");

  REQUIRE(response["result"] == "success");

  client.Stop();
}

TEST_CASE(
    "Client sends notification without expecting a response", "[Client]") {
  auto transport = std::make_unique<MockTransport>();
  MockTransport *transportPtr = transport.get();

  Client client(std::move(transport));
  client.Start();

  client.SendNotification("notify_event", Json({{"param1", "value1"}}));

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

  auto response = client.SendMethodCall("getData");

  REQUIRE(response.contains("result"));
  REQUIRE(response["result"].contains("data"));
  REQUIRE(response["result"]["data"] == "success");

  client.Stop();
}
