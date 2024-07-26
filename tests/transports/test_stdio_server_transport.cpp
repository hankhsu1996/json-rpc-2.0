#include <iostream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/core/dispatcher.hpp"
#include "jsonrpc/transports/stdio_server_transport.hpp"

#include "../common/test_utilities.hpp"

using namespace jsonrpc;

TEST_CASE(
    "StdioServerTransport dispatches requests", "[StdioServerTransport]") {
  Dispatcher dispatcher;
  dispatcher.RegisterMethodCall(
      "testMethod", [](const std::optional<Json> &params) -> Json {
        if (params.has_value()) {
          REQUIRE(params.value()["param1"] == 1);
        }
        return Json{{"result", "success"}};
      });

  StdioServerTransport serverTransport;
  serverTransport.SetDispatcher(&dispatcher);

  std::string input =
      R"({"jsonrpc":"2.0","method":"testMethod","params":{"param1":1},"id":1})";
  std::istringstream in(input);
  std::ostringstream out;
  RedirectIO redirect(in, out);

  serverTransport.Start();

  std::string output = out.str();
  Json outputJson = Json::parse(output);
  Json expectedJson = {{"result", "success"}, {"id", 1}};

  REQUIRE(outputJson == expectedJson);
}

TEST_CASE(
    "StdioServerTransport handles invalid input", "[StdioServerTransport]") {
  Dispatcher dispatcher;
  StdioServerTransport serverTransport;
  serverTransport.SetDispatcher(&dispatcher);

  std::string input = "invalid Json";
  std::istringstream in(input);
  std::ostringstream out;
  RedirectIO redirect(in, out);

  serverTransport.Start();

  std::string output = out.str();
  Json outputJson = Json::parse(output);
  Json expectedJson = {
      {"error", {{"code", -32700}, {"message", "Parse error"}}},
      {"id", nullptr}};

  REQUIRE(outputJson == expectedJson);
}
