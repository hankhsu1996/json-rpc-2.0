#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include "jsonrpc/server/response.hpp"

TEST_CASE("Response success creation and serialization", "[Response]") {
  nlohmann::json result = {{"data", "value"}};
  std::optional<int> id = 1;

  jsonrpc::server::Response response =
      jsonrpc::server::Response::CreateResult(result, id);
  nlohmann::json response_json = response.ToJson();

  REQUIRE(response_json["result"] == result);
  REQUIRE(response_json["id"] == id.value());
}

TEST_CASE("Response library error creation and serialization", "[Response]") {
  std::string message = "Method not found";
  int code = -32601;
  std::optional<int> id = 1;

  nlohmann::json response_json =
      jsonrpc::server::Response::CreateLibError(
          jsonrpc::server::LibErrorKind::kMethodNotFound, id)
          .ToJson();

  REQUIRE(response_json["error"]["message"] == message);
  REQUIRE(response_json["error"]["code"] == code);
  REQUIRE(response_json["id"] == id.value());
}

TEST_CASE("Response user error creation and serialization", "[Response]") {
  nlohmann::json error = {{"code", -32602}, {"message", "Invalid params"}};
  std::optional<int> id = 1;

  jsonrpc::server::Response response =
      jsonrpc::server::Response::CreateUserError(error, id);
  nlohmann::json response_json = response.ToJson();

  REQUIRE(response_json["error"] == error);
  REQUIRE(response_json["id"] == id.value());
}

TEST_CASE(
    "Response validation with missing 'code' or 'message' in error",
    "[Response]") {
  nlohmann::json invalid_error_response = {
      {"error", {{"code", -32603}}}, {"id", 1}};
  REQUIRE_THROWS_AS(
      jsonrpc::server::Response::FromUserResponse(invalid_error_response, 1),
      std::invalid_argument);

  nlohmann::json valid_error_response = {
      {"error", {{"code", -32603}, {"message", "Internal error"}}}, {"id", 1}};
  REQUIRE_NOTHROW(
      jsonrpc::server::Response::FromUserResponse(valid_error_response, 1));
}

TEST_CASE("Response creation without id", "[Response]") {
  nlohmann::json result = {{"data", "value"}};

  jsonrpc::server::Response response =
      jsonrpc::server::Response::CreateResult(result, std::nullopt);
  nlohmann::json response_json = response.ToJson();

  REQUIRE(response_json["result"] == result);
  REQUIRE(response_json.find("id") == response_json.end());
}

TEST_CASE("Library error response creation without id", "[Response]") {
  std::string message = "Parse error";
  int code = -32700;

  nlohmann::json response_json = jsonrpc::server::Response::CreateLibError(
                                     jsonrpc::server::LibErrorKind::kParseError)
                                     .ToJson();

  REQUIRE(response_json["error"]["message"] == message);
  REQUIRE(response_json["error"]["code"] == code);
  REQUIRE(response_json["id"] == nullptr);
}

TEST_CASE("User error response creation without id", "[Response]") {
  nlohmann::json error = {{"code", -32602}, {"message", "Invalid params"}};

  jsonrpc::server::Response response =
      jsonrpc::server::Response::CreateUserError(error, std::nullopt);
  nlohmann::json response_json = response.ToJson();

  REQUIRE(response_json["error"] == error);
  REQUIRE(response_json.find("id") == response_json.end());
}
