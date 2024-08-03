#include <iostream>
#include <numeric>

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include "jsonrpc/server/dispatcher.hpp"
#include "jsonrpc/server/request.hpp"
#include "jsonrpc/server/response.hpp"

using namespace jsonrpc::server;

// Helper function to create a Dispatcher object
std::unique_ptr<Dispatcher> CreateDispatcher(bool enableMultithreading,
    size_t numThreads = std::thread::hardware_concurrency()) {
  return std::make_unique<Dispatcher>(enableMultithreading, numThreads);
}

// Helper function to register common method handlers
void RegisterCommonHandlers(Dispatcher &dispatcher) {
  auto subtract =
      [](const std::optional<nlohmann::json> &params) -> nlohmann::json {
    int result;
    if (params && params->is_array()) {
      result = params.value()[0].get<int>() - params.value()[1].get<int>();
    } else {
      result = params.value()["minuend"].get<int>() -
               params.value()["subtrahend"].get<int>();
    }
    return nlohmann::json{{"result", result}};
  };

  auto sum = [](const std::optional<nlohmann::json> &params) -> nlohmann::json {
    int result = 0;
    if (params) {
      result = std::accumulate(params->begin(), params->end(), 0,
          [](int sum, const nlohmann::json &param) {
            return sum + param.get<int>();
          });
    }
    return nlohmann::json{{"result", result}};
  };

  auto get_data = [](const std::optional<nlohmann::json> &) -> nlohmann::json {
    return nlohmann::json{{"result", nlohmann::json::array({"hello", 5})}};
  };

  auto notify_hello = [](const std::optional<nlohmann::json> &params) {
    if (params) {
      REQUIRE(params.value()[0] == 7);
    }
  };

  auto notify_sum = [](const std::optional<nlohmann::json> &params) {
    if (params) {
      REQUIRE(params.value().size() == 3);
    }
  };

  dispatcher.RegisterMethodCall("subtract", subtract);
  dispatcher.RegisterMethodCall("sum", sum);
  dispatcher.RegisterMethodCall("get_data", get_data);
  dispatcher.RegisterNotification("notify_hello", notify_hello);
  dispatcher.RegisterNotification("notify_sum", notify_sum);
}

TEST_CASE("RPC call with positional parameters", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("subtract 42, 23") {
    nlohmann::json requestJson = {{"jsonrpc", "2.0"}, {"method", "subtract"},
        {"params", {42, 23}}, {"id", 1}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(responseStr.has_value());
    nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
    REQUIRE(responseJson["jsonrpc"] == "2.0");
    REQUIRE(responseJson["result"] == 19);
    REQUIRE(responseJson["id"] == 1);
  }

  SECTION("subtract 23, 42") {
    nlohmann::json requestJson = {{"jsonrpc", "2.0"}, {"method", "subtract"},
        {"params", {23, 42}}, {"id", 2}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(responseStr.has_value());
    nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
    REQUIRE(responseJson["jsonrpc"] == "2.0");
    REQUIRE(responseJson["result"] == -19);
    REQUIRE(responseJson["id"] == 2);
  }
}

TEST_CASE("RPC call with named parameters", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("subtract named parameters 1") {
    nlohmann::json requestJson = {{"jsonrpc", "2.0"}, {"method", "subtract"},
        {"params", {{"subtrahend", 23}, {"minuend", 42}}}, {"id", 3}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(responseStr.has_value());
    nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
    REQUIRE(responseJson["jsonrpc"] == "2.0");
    REQUIRE(responseJson["result"] == 19);
    REQUIRE(responseJson["id"] == 3);
  }

  SECTION("subtract named parameters 2") {
    nlohmann::json requestJson = {{"jsonrpc", "2.0"}, {"method", "subtract"},
        {"params", {{"minuend", 42}, {"subtrahend", 23}}}, {"id", 4}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(responseStr.has_value());
    nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
    REQUIRE(responseJson["jsonrpc"] == "2.0");
    REQUIRE(responseJson["result"] == 19);
    REQUIRE(responseJson["id"] == 4);
  }
}

TEST_CASE("Notification", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("update notification") {
    nlohmann::json requestJson = {
        {"jsonrpc", "2.0"}, {"method", "update"}, {"params", {1, 2, 3, 4, 5}}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(!responseStr.has_value());
  }

  SECTION("foobar notification") {
    nlohmann::json requestJson = {{"jsonrpc", "2.0"}, {"method", "foobar"}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(!responseStr.has_value());
  }
}

TEST_CASE("RPC call of non-existent method", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  nlohmann::json requestJson = {
      {"jsonrpc", "2.0"}, {"method", "foobar"}, {"id", "1"}};
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
  REQUIRE(responseJson["jsonrpc"] == "2.0");
  REQUIRE(responseJson["error"]["code"] == -32601); // Method not found
  REQUIRE(responseJson["error"]["message"] == "Method not found");
  REQUIRE(responseJson["id"] == "1");
}

TEST_CASE("RPC call with invalid JSON", "[Dispatcher]") {
  Dispatcher dispatcher;

  std::string invalidRequest =
      R"({"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz])";
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(invalidRequest);
  REQUIRE(responseStr.has_value());
  nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
  REQUIRE(responseJson["jsonrpc"] == "2.0");
  REQUIRE(responseJson["error"]["code"] == -32700); // Parse error
  REQUIRE(responseJson["error"]["message"] == "Parse error");
  REQUIRE(responseJson["id"] == nullptr);
}

TEST_CASE("RPC call with invalid ServerRequest object", "[Dispatcher]") {
  Dispatcher dispatcher;

  nlohmann::json requestJson = {
      {"jsonrpc", "2.0"}, {"method", 1}, {"params", "bar"}};
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
  REQUIRE(responseJson["jsonrpc"] == "2.0");
  REQUIRE(responseJson["error"]["code"] == -32600); // Invalid Request
  REQUIRE(responseJson["error"]["message"] == "Invalid Request");
  REQUIRE(responseJson["id"] == nullptr);
}

TEST_CASE("RPC call Batch, invalid JSON", "[Dispatcher]") {
  Dispatcher dispatcher;

  std::string invalidBatchRequest =
      R"([{"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id":
      "1"},{"jsonrpc": "2.0", "method"])";
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(invalidBatchRequest);
  REQUIRE(responseStr.has_value());
  nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
  REQUIRE(responseJson["jsonrpc"] == "2.0");
  REQUIRE(responseJson["error"]["code"] == -32700); // Parse error
  REQUIRE(responseJson["error"]["message"] == "Parse error");
  REQUIRE(responseJson["id"] == nullptr);
}

TEST_CASE("RPC call with an empty Array", "[Dispatcher]") {
  Dispatcher dispatcher;

  nlohmann::json requestJson = nlohmann::json::array();
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
  REQUIRE(responseJson["jsonrpc"] == "2.0");
  REQUIRE(responseJson["error"]["code"] == -32600); // Invalid Request
  REQUIRE(responseJson["error"]["message"] == "Invalid Request");
  REQUIRE(responseJson["id"] == nullptr);
}

TEST_CASE("RPC call with an invalid Batch (but not empty)", "[Dispatcher]") {
  Dispatcher dispatcher;

  nlohmann::json requestJson = nlohmann::json::array({1});
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
  REQUIRE(responseJson.is_array());
  REQUIRE(responseJson.size() == 1);
  REQUIRE(responseJson[0]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[0]["error"]["code"] == -32600); // Invalid Request
  REQUIRE(responseJson[0]["error"]["message"] == "Invalid Request");
  REQUIRE(responseJson[0]["id"] == nullptr);
}

TEST_CASE("RPC call with invalid Batch", "[Dispatcher]") {
  Dispatcher dispatcher;

  nlohmann::json requestJson = nlohmann::json::array({1, 2, 3});
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());
  REQUIRE(responseJson.is_array());
  REQUIRE(responseJson.size() == 3);
  for (const auto &response : responseJson) {
    REQUIRE(response["jsonrpc"] == "2.0");
    REQUIRE(response["error"]["code"] == -32600); // Invalid Request
    REQUIRE(response["error"]["message"] == "Invalid Request");
    REQUIRE(response["id"] == nullptr);
  }
}

TEST_CASE("RPC call Batch", "[Dispatcher]") {
  auto dispatcher = CreateDispatcher(true);
  RegisterCommonHandlers(*dispatcher);

  // clang-format off
  nlohmann::json requestJson = nlohmann::json::array({
    {{"jsonrpc", "2.0"}, {"method", "sum"}, {"params", {1, 2, 4}}, {"id",
    "1"}},
    {{"jsonrpc", "2.0"}, {"method", "notify_hello"}, {"params", {7}}},
    {{"jsonrpc", "2.0"}, {"method", "subtract"}, {"params", {42, 23}}, {"id",
    "2"}},
    {{"foo", "boo"}},
    {{"jsonrpc", "2.0"}, {"method", "foo.get"}, {"params", {{"name",
    "myself"}}}, {"id", "5"}},
    {{"jsonrpc", "2.0"}, {"method", "get_data"}, {"id", "9"}}
  });
  // clang-format on

  std::optional<std::string> responseStr =
      dispatcher->DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  nlohmann::json responseJson = nlohmann::json::parse(responseStr.value());

  REQUIRE(responseJson.is_array());
  REQUIRE(responseJson.size() == 5); // 6 requests but one is a notification

  REQUIRE(responseJson[0]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[0]["result"] == 7);
  REQUIRE(responseJson[0]["id"] == "1");

  REQUIRE(responseJson[1]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[1]["result"] == 19);
  REQUIRE(responseJson[1]["id"] == "2");

  REQUIRE(responseJson[2]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[2]["error"]["code"] == -32600); // Invalid Request
  REQUIRE(responseJson[2]["error"]["message"] == "Invalid Request");
  REQUIRE(responseJson[2]["id"] == nullptr);

  REQUIRE(responseJson[3]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[3]["error"]["code"] == -32601); // Method not found
  REQUIRE(responseJson[3]["error"]["message"] == "Method not found");
  REQUIRE(responseJson[3]["id"] == "5");

  REQUIRE(responseJson[4]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[4]["result"] == nlohmann::json::array({"hello", 5}));
  REQUIRE(responseJson[4]["id"] == "9");

  dispatcher = CreateDispatcher(false);
  RegisterCommonHandlers(*dispatcher);
  responseStr = dispatcher->DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  responseJson = nlohmann::json::parse(responseStr.value());
  REQUIRE(responseJson.is_array());
  REQUIRE(responseJson.size() == 5); // 6 requests but one is a notification

  REQUIRE(responseJson[0]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[0]["result"] == 7);
  REQUIRE(responseJson[0]["id"] == "1");

  REQUIRE(responseJson[1]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[1]["result"] == 19);
  REQUIRE(responseJson[1]["id"] == "2");

  REQUIRE(responseJson[2]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[2]["error"]["code"] == -32600); // Invalid Request
  REQUIRE(responseJson[2]["error"]["message"] == "Invalid Request");
  REQUIRE(responseJson[2]["id"] == nullptr);

  REQUIRE(responseJson[3]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[3]["error"]["code"] == -32601); // Method not found
  REQUIRE(responseJson[3]["error"]["message"] == "Method not found");
  REQUIRE(responseJson[3]["id"] == "5");

  REQUIRE(responseJson[4]["jsonrpc"] == "2.0");
  REQUIRE(responseJson[4]["result"] == nlohmann::json::array({"hello", 5}));
  REQUIRE(responseJson[4]["id"] == "9");
}

TEST_CASE("RPC call Batch (all notifications)", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  nlohmann::json requestJson = nlohmann::json::array(
      {{{"jsonrpc", "2.0"}, {"method", "notify_sum"}, {"params", {1, 2, 4}}},
          {{"jsonrpc", "2.0"}, {"method", "notify_hello"}, {"params", {7}}}});
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(!responseStr.has_value());
}
