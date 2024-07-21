#include <catch2/catch_test_macros.hpp>

#include "json_rpc/core/dispatcher.h"
#include "json_rpc/core/request.h"
#include "json_rpc/core/response.h"

using namespace json_rpc;

TEST_CASE("Dispatch method call", "[Dispatcher]") {
  Dispatcher dispatcher;

  // Register a method call handler
  dispatcher.RegisterMethodCall("testMethod", [](const Json &params) -> Json {
    REQUIRE(params["param1"] == 1);
    return Json{{"result", "success"}};
  });

  // Create a request
  Json requestJson = {{"jsonrpc", "2.0"}, {"method", "testMethod"},
      {"params", {{"param1", 1}}}, {"id", 1}};

  // Dispatch the request
  std::optional<std::string> responseStr =
      dispatcher.Dispatch(requestJson.dump());
  REQUIRE(responseStr.has_value());

  // Parse and verify the response
  Json responseJson = Json::parse(responseStr.value());
  REQUIRE(responseJson["result"] == "success");
  REQUIRE(responseJson["id"] == 1);
}

TEST_CASE("Dispatch notification", "[Dispatcher]") {
  Dispatcher dispatcher;

  // Register a notification handler
  dispatcher.RegisterNotification("testNotification",
      [](const Json &params) { REQUIRE(params["param1"] == 1); });

  // Create a notification request
  Json requestJson = {{"jsonrpc", "2.0"}, {"method", "testNotification"},
      {"params", {{"param1", 1}}}};

  // Dispatch the notification
  std::optional<std::string> responseStr =
      dispatcher.Dispatch(requestJson.dump());
  REQUIRE(!responseStr.has_value());
}

TEST_CASE("Method not found", "[Dispatcher]") {
  Dispatcher dispatcher;

  // Create a request for an unregistered method
  Json requestJson = {{"jsonrpc", "2.0"}, {"method", "unknownMethod"},
      {"params", {{"param1", 1}}}, {"id", 1}};

  // Dispatch the request
  std::optional<std::string> responseStr =
      dispatcher.Dispatch(requestJson.dump());
  REQUIRE(responseStr.has_value());

  // Parse and verify the response
  Json responseJson = Json::parse(responseStr.value());
  REQUIRE(responseJson["error"]["code"] == -32601); // Method not found
  REQUIRE(responseJson["error"]["message"] == "Method not found");
  REQUIRE(responseJson["id"] == 1);
}

TEST_CASE("Parse error", "[Dispatcher]") {
  Dispatcher dispatcher;

  // Create an invalid JSON request
  std::string invalidRequest = "{invalid json}";

  // Dispatch the request
  std::optional<std::string> responseStr = dispatcher.Dispatch(invalidRequest);
  REQUIRE(responseStr.has_value());

  // Parse and verify the response
  Json responseJson = Json::parse(responseStr.value());
  REQUIRE(responseJson["error"]["code"] == -32700); // Parse error
  REQUIRE(responseJson["error"]["message"] == "Parse error");
}
