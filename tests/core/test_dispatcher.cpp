#include <iostream>
#include <numeric>

#include <catch2/catch_test_macros.hpp>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/request.hpp"
#include "json_rpc/core/response.hpp"

using namespace json_rpc;

// Helper function to register common method handlers
void RegisterCommonHandlers(Dispatcher &dispatcher) {
  dispatcher.RegisterMethodCall(
      "subtract", [](const std::optional<Json> &params) -> Json {
        int result;
        if (params.has_value() && params->is_array()) {
          result = params.value()[0].get<int>() - params.value()[1].get<int>();
        } else {
          result = params.value()["minuend"].get<int>() -
                   params.value()["subtrahend"].get<int>();
        }
        return Json{{"result", result}};
      });

  dispatcher.RegisterMethodCall(
      "sum", [](const std::optional<Json> &params) -> Json {
        int result = 0;
        if (params) {
          result = std::accumulate(params->begin(), params->end(), 0,
              [](int sum, const Json &param) {
                return sum + param.get<int>();
              });
        }
        return Json{{"result", result}};
      });

  dispatcher.RegisterMethodCall(
      "get_data", [](const std::optional<Json> &) -> Json {
        return Json{{"result", Json::array({"hello", 5})}};
      });

  dispatcher.RegisterNotification(
      "notify_hello", [](const std::optional<Json> &params) {
        if (params) {
          REQUIRE(params.value()[0] == 7);
        }
      });

  dispatcher.RegisterNotification(
      "notify_sum", [](const std::optional<Json> &params) {
        if (params) {
          REQUIRE(params.value().size() == 3);
        }
      });
}

TEST_CASE("RPC call with positional parameters", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("subtract 42, 23") {
    Json requestJson = {{"jsonrpc", "2.0"}, {"method", "subtract"},
        {"params", {42, 23}}, {"id", 1}};
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  Json responseJson = Json::parse(responseStr.value());
    REQUIRE(responseJson["result"] == 19);
  REQUIRE(responseJson["id"] == 1);
}

  SECTION("subtract 23, 42") {
    Json requestJson = {{"jsonrpc", "2.0"}, {"method", "subtract"},
        {"params", {23, 42}}, {"id", 2}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(responseStr.has_value());
    Json responseJson = Json::parse(responseStr.value());
    REQUIRE(responseJson["result"] == -19);
    REQUIRE(responseJson["id"] == 2);
  }
}

TEST_CASE("RPC call with named parameters", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("subtract named parameters 1") {
    Json requestJson = {{"jsonrpc", "2.0"}, {"method", "subtract"},
        {"params", {{"subtrahend", 23}, {"minuend", 42}}}, {"id", 3}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(responseStr.has_value());
    Json responseJson = Json::parse(responseStr.value());
    REQUIRE(responseJson["result"] == 19);
    REQUIRE(responseJson["id"] == 3);
  }

  SECTION("subtract named parameters 2") {
    Json requestJson = {{"jsonrpc", "2.0"}, {"method", "subtract"},
        {"params", {{"minuend", 42}, {"subtrahend", 23}}}, {"id", 4}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(responseStr.has_value());
    Json responseJson = Json::parse(responseStr.value());
    REQUIRE(responseJson["result"] == 19);
    REQUIRE(responseJson["id"] == 4);
  }
}

TEST_CASE("Notification", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("update notification") {
    Json requestJson = {
        {"jsonrpc", "2.0"}, {"method", "update"}, {"params", {1, 2, 3, 4, 5}}};
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(!responseStr.has_value());
}

  SECTION("foobar notification") {
    Json requestJson = {{"jsonrpc", "2.0"}, {"method", "foobar"}};
    std::optional<std::string> responseStr =
        dispatcher.DispatchRequest(requestJson.dump());
    REQUIRE(!responseStr.has_value());
  }
}

TEST_CASE("RPC call of non-existent method", "[Dispatcher]") {
  Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  Json requestJson = {{"jsonrpc", "2.0"}, {"method", "foobar"}, {"id", "1"}};
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  Json responseJson = Json::parse(responseStr.value());
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
  Json responseJson = Json::parse(responseStr.value());
  REQUIRE(responseJson["error"]["code"] == -32700); // Parse error
  REQUIRE(responseJson["error"]["message"] == "Parse error");
  REQUIRE(responseJson["id"] == nullptr);
}

TEST_CASE("RPC call with invalid Request object", "[Dispatcher]") {
  Dispatcher dispatcher;

  Json requestJson = {{"jsonrpc", "2.0"}, {"method", 1}, {"params", "bar"}};
  std::optional<std::string> responseStr =
      dispatcher.DispatchRequest(requestJson.dump());
  REQUIRE(responseStr.has_value());
  Json responseJson = Json::parse(responseStr.value());
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
  Json responseJson = Json::parse(responseStr.value());
  REQUIRE(responseJson["error"]["code"] == -32700); // Parse error
  REQUIRE(responseJson["error"]["message"] == "Parse error");
  REQUIRE(responseJson["id"] == nullptr);
}
