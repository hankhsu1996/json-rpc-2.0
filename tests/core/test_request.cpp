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

// Test deserialization from JSON without params
TEST_CASE("Request deserialization from JSON without params", "[Request]") {
  std::string method = "testMethod";
  int id = 1;

  Json jsonObj = {{"jsonrpc", "2.0"}, {"method", method}, {"id", id}};

  Request request = Request::FromJson(jsonObj);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(!request.GetParams().has_value());
  REQUIRE(request.GetId().has_value());
  REQUIRE(request.GetId().value() == id);
}
