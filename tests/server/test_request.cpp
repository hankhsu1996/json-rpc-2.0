#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/server/request.hpp"

// Test the Request constructor and accessors
TEST_CASE("Request constructor and accessors", "[Request]") {
  std::string method = "testMethod";
  nlohmann::json params = {{"param1", 1}, {"param2", 2}};
  std::optional<int> id = 1;

  jsonrpc::server::Request request(method, params, id);

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

  jsonrpc::server::Request request(method, params, id);
  nlohmann::json json_obj = request.ToJson();

  REQUIRE(json_obj["jsonrpc"] == "2.0");
  REQUIRE(json_obj["method"] == method);
  REQUIRE(json_obj["params"] == params);
  REQUIRE(json_obj["id"] == id.value());
}

// Test deserialization from JSON
TEST_CASE("Request deserialization from JSON", "[Request]") {
  std::string method = "testMethod";
  nlohmann::json params = {{"param1", 1}, {"param2", 2}};
  int id = 1;

  nlohmann::json json_obj = {
      {"jsonrpc", "2.0"}, {"method", method}, {"params", params}, {"id", id}};

  jsonrpc::server::Request request =
      jsonrpc::server::Request::FromJson(json_obj);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(request.GetParams() == params);
  REQUIRE(request.GetId().has_value());
  REQUIRE(request.GetId().value() == id);
}

// Test deserialization from JSON without id
TEST_CASE("Request deserialization from JSON without id", "[Request]") {
  std::string method = "testMethod";
  nlohmann::json params = {{"param1", 1}, {"param2", 2}};

  nlohmann::json json_obj = {
      {"jsonrpc", "2.0"}, {"method", method}, {"params", params}};

  jsonrpc::server::Request request =
      jsonrpc::server::Request::FromJson(json_obj);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(request.GetParams() == params);
  REQUIRE(!request.GetId().has_value());
}

// Test deserialization from JSON without params
TEST_CASE("Request deserialization from JSON without params", "[Request]") {
  std::string method = "testMethod";
  int id = 1;

  nlohmann::json json_obj = {
      {"jsonrpc", "2.0"}, {"method", method}, {"id", id}};

  jsonrpc::server::Request request =
      jsonrpc::server::Request::FromJson(json_obj);

  REQUIRE(request.GetMethod() == method);
  REQUIRE(!request.GetParams().has_value());
  REQUIRE(request.GetId().has_value());
  REQUIRE(request.GetId().value() == id);
}
