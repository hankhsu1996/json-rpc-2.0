#include <catch2/catch_test_macros.hpp>

#include "json_rpc/core/request.hpp"

using namespace json_rpc;

// Test the Request constructor and accessors
TEST_CASE("Request constructor and accessors", "[Request]") {
  std::string method = "testMethod";
  Json params = {{"param1", 1}, {"param2", 2}};
  std::optional<int> id = 1;

  Request request(method, params, id);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(request.GetParams() == params);
  REQUIRE(request.GetId().has_value());
  REQUIRE(request.GetId().value() == id.value());
}

// Test serialization to JSON
TEST_CASE("Request serialization to JSON", "[Request]") {
  std::string method = "testMethod";
  Json params = {{"param1", 1}, {"param2", 2}};
  std::optional<int> id = 1;

  Request request(method, params, id);
  Json jsonObj = request.ToJson();

  REQUIRE(jsonObj["jsonrpc"] == "2.0");
  REQUIRE(jsonObj["method"] == method);
  REQUIRE(jsonObj["params"] == params);
  REQUIRE(jsonObj["id"] == id.value());
}

// Test deserialization from JSON
TEST_CASE("Request deserialization from JSON", "[Request]") {
  std::string method = "testMethod";
  Json params = {{"param1", 1}, {"param2", 2}};
  int id = 1;

  Json jsonObj = {
      {"jsonrpc", "2.0"}, {"method", method}, {"params", params}, {"id", id}};

  Request request = Request::FromJson(jsonObj);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(request.GetParams() == params);
  REQUIRE(request.GetId().has_value());
  REQUIRE(request.GetId().value() == id);
}

// Test deserialization from JSON without id
TEST_CASE("Request deserialization from JSON without id", "[Request]") {
  std::string method = "testMethod";
  Json params = {{"param1", 1}, {"param2", 2}};

  Json jsonObj = {{"jsonrpc", "2.0"}, {"method", method}, {"params", params}};

  Request request = Request::FromJson(jsonObj);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(request.GetParams() == params);
  REQUIRE(!request.GetId().has_value());
}

// Test deserialization with invalid JSON-RPC version
TEST_CASE(
    "Request deserialization with invalid JSON-RPC version", "[Request]") {
  std::string method = "testMethod";
  Json params = {{"param1", 1}, {"param2", 2}};
  int id = 1;

  Json jsonObj = {{"jsonrpc", "1.0"}, // Invalid version
      {"method", method}, {"params", params}, {"id", id}};

  REQUIRE_THROWS_AS(Request::FromJson(jsonObj), std::invalid_argument);
}

// Test deserialization with missing fields
TEST_CASE("Request deserialization with missing fields", "[Request]") {
  // Missing 'params' field
  Json jsonObj1 = {{"jsonrpc", "2.0"}, {"method", "testMethod"}};
  REQUIRE_THROWS_AS(Request::FromJson(jsonObj1), std::invalid_argument);

  // Missing 'method' field
  Json jsonObj2 = {
      {"jsonrpc", "2.0"}, {"params", {{"param1", 1}, {"param2", 2}}}};
  REQUIRE_THROWS_AS(Request::FromJson(jsonObj2), std::invalid_argument);

  // Missing both 'method' and 'params' fields
  Json jsonObj3 = {{"jsonrpc", "2.0"}};
  REQUIRE_THROWS_AS(Request::FromJson(jsonObj3), std::invalid_argument);
}