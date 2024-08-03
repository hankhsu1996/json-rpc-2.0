#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/server/request.hpp"

using namespace jsonrpc::server;

// Test the Request constructor and accessors
TEST_CASE("Request constructor and accessors", "[Request]") {
  std::string method = "testMethod";
  nlohmann::json params = {{"param1", 1}, {"param2", 2}};
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
  nlohmann::json params = {{"param1", 1}, {"param2", 2}};
  std::optional<int> id = 1;

  Request request(method, params, id);
  nlohmann::json jsonObj = request.ToJson();

  REQUIRE(jsonObj["jsonrpc"] == "2.0");
  REQUIRE(jsonObj["method"] == method);
  REQUIRE(jsonObj["params"] == params);
  REQUIRE(jsonObj["id"] == id.value());
}

// Test deserialization from JSON
TEST_CASE("Request deserialization from JSON", "[Request]") {
  std::string method = "testMethod";
  nlohmann::json params = {{"param1", 1}, {"param2", 2}};
  int id = 1;

  nlohmann::json jsonObj = {
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
  nlohmann::json params = {{"param1", 1}, {"param2", 2}};

  nlohmann::json jsonObj = {
      {"jsonrpc", "2.0"}, {"method", method}, {"params", params}};

  Request request = Request::FromJson(jsonObj);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(request.GetParams() == params);
  REQUIRE(!request.GetId().has_value());
}

// Test deserialization from JSON without params
TEST_CASE("Request deserialization from JSON without params", "[Request]") {
  std::string method = "testMethod";
  int id = 1;

  nlohmann::json jsonObj = {{"jsonrpc", "2.0"}, {"method", method}, {"id", id}};

  Request request = Request::FromJson(jsonObj);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(!request.GetParams().has_value());
  REQUIRE(request.GetId().has_value());
  REQUIRE(request.GetId().value() == id);
}
