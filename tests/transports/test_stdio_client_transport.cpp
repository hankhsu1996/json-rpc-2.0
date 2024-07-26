#include <iostream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/core/dispatcher.hpp"
#include "jsonrpc/transports/stdio_client_transport.hpp"

#include "../common/test_utilities.hpp"

using namespace jsonrpc;

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
