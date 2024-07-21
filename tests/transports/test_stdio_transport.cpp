#include <iostream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include "json_rpc/core/dispatcher.h"
#include "json_rpc/transports/stdio_transport.h"

#include "../common/test_utilities.h"

using namespace json_rpc;

TEST_CASE("StdioClientTransport sends method call and receives response",
    "[StdioClientTransport]") {
  std::string input = R"({"jsonrpc":"2.0","result":"success","id":1})";
  std::istringstream in(input);
  RedirectIO redirect(in, std::cout);

  StdioClientTransport clientTransport;
  Json request = {{"jsonrpc", "2.0"}, {"method", "testMethod"},
      {"params", {{"param1", 1}}}, {"id", 1}};

  Json response = clientTransport.SendMethodCall(request);

  REQUIRE(response["result"] == "success");
  REQUIRE(response["id"] == 1);
}

TEST_CASE("StdioClientTransport sends notification", "[StdioClientTransport]") {
  std::ostringstream out;
  RedirectIO redirect(std::cin, out);

  StdioClientTransport clientTransport;
  Json notification = {{"jsonrpc", "2.0"}, {"method", "testNotification"},
      {"params", {{"param1", 1}}}};

  clientTransport.SendNotification(notification);

  std::string output = out.str();
  REQUIRE(output == notification.dump() + "\n");
}

TEST_CASE(
    "StdioServerTransport dispatches requests", "[StdioServerTransport]") {
  Dispatcher dispatcher;
  dispatcher.RegisterMethodCall("testMethod", [](const Json &params) -> Json {
    REQUIRE(params["param1"] == 1);
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
      {"error", {{"code", -32700}, {"message", "Parse error"}}}};

  REQUIRE(outputJson == expectedJson);
}
