#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/core/response.hpp"

using namespace jsonrpc;

TEST_CASE("Response success creation and serialization", "[Response]") {
  Json result = {{"data", "value"}};
  std::optional<int> id = 1;

  Response response = Response::CreateResult(result, id);
  Json responseJson = response.ToJson();

  REQUIRE(responseJson["result"] == result);
  REQUIRE(responseJson["id"] == id.value());
}

TEST_CASE("Response library error creation and serialization", "[Response]") {
  std::string message = "Method not found";
  int code = -32601;
  std::optional<int> id = 1;

  Json responseJson =
      Response::CreateLibError(LibErrorKind::MethodNotFound, id).ToJson();

  REQUIRE(responseJson["error"]["message"] == message);
  REQUIRE(responseJson["error"]["code"] == code);
  REQUIRE(responseJson["id"] == id.value());
}

TEST_CASE("Response user error creation and serialization", "[Response]") {
  Json error = {{"code", -32602}, {"message", "Invalid params"}};
  std::optional<int> id = 1;

  Response response = Response::CreateUserError(error, id);
  Json responseJson = response.ToJson();

  REQUIRE(responseJson["error"] == error);
  REQUIRE(responseJson["id"] == id.value());
}

TEST_CASE("Response validation with missing 'code' or 'message' in error",
    "[Response]") {
  Json invalidErrorResponse = {{"error", {{"code", -32603}}}, {"id", 1}};
  REQUIRE_THROWS_AS(Response::FromUserResponse(invalidErrorResponse, 1),
      std::invalid_argument);

  Json validErrorResponse = {
      {"error", {{"code", -32603}, {"message", "Internal error"}}}, {"id", 1}};
  REQUIRE_NOTHROW(Response::FromUserResponse(validErrorResponse, 1));
}

TEST_CASE("Response creation without id", "[Response]") {
  Json result = {{"data", "value"}};

  Response response = Response::CreateResult(result, std::nullopt);
  Json responseJson = response.ToJson();

  REQUIRE(responseJson["result"] == result);
  REQUIRE(responseJson.find("id") == responseJson.end());
}

TEST_CASE("Library error response creation without id", "[Response]") {
  std::string message = "Parse error";
  int code = -32700;

  Json responseJson =
      Response::CreateLibError(LibErrorKind::ParseError).ToJson();

  REQUIRE(responseJson["error"]["message"] == message);
  REQUIRE(responseJson["error"]["code"] == code);
  REQUIRE(responseJson["id"] == nullptr);
}

TEST_CASE("User error response creation without id", "[Response]") {
  Json error = {{"code", -32602}, {"message", "Invalid params"}};

  Response response = Response::CreateUserError(error, std::nullopt);
  Json responseJson = response.ToJson();

  REQUIRE(responseJson["error"] == error);
  REQUIRE(responseJson.find("id") == responseJson.end());
}
