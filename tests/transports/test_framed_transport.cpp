#include <iostream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include "json_rpc/transports/framed_transport.h"

#include "../common/test_utilities.h"

using namespace json_rpc;

// Derived test class to expose protected methods
class TestFramedTransport : public FramedTransport {
public:
  using FramedTransport::ReceiveMessage;
  using FramedTransport::SendMessage;
};

TEST_CASE("FramedTransport sends message", "[FramedTransport]") {
  std::ostringstream out;
  RedirectIO redirect(std::cin, out);

  TestFramedTransport transport;
  std::string message =
      R"({"jsonrpc":"2.0","method":"testMethod","params":{"param1":1},"id":1})";
  transport.SendMessage(message);

  std::string expected_output =
      "Content-Length: " + std::to_string(message.size()) +
      "\r\nContent-Type: application/vscode-jsonrpc; charset=utf-8\r\n\r\n" +
      message;
  REQUIRE(out.str() == expected_output);
}

TEST_CASE("FramedTransport receives message", "[FramedTransport]") {
  std::string input =
      "Content-Length: 68\r\n"
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n"
      R"({"jsonrpc":"2.0","method":"testMethod","params":{"param1":1},"id":1})";
  std::istringstream in(input);
  RedirectIO redirect(in, std::cout);

  TestFramedTransport transport;
  std::string message = transport.ReceiveMessage();

  std::string expected_message =
      R"({"jsonrpc":"2.0","method":"testMethod","params":{"param1":1},"id":1})";
  REQUIRE(message == expected_message);
}

TEST_CASE("FramedTransport throws on invalid message", "[FramedTransport]") {
  std::string input =
      "Content-Length: invalid\r\n"
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n";
  std::istringstream in(input);
  RedirectIO redirect(in, std::cout);

  TestFramedTransport transport;

  try {
    transport.ReceiveMessage();
    FAIL("Expected std::runtime_error");
  } catch (const std::runtime_error &e) {
    REQUIRE(std::string(e.what()) == "Invalid Content-Length value");
  } catch (...) {
    FAIL("Expected std::runtime_error");
  }
}

TEST_CASE(
    "FramedTransport throws on missing content length", "[FramedTransport]") {
  std::string input =
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n";
  std::istringstream in(input);
  RedirectIO redirect(in, std::cout);

  TestFramedTransport transport;

  try {
    transport.ReceiveMessage();
    FAIL("Expected std::runtime_error");
  } catch (const std::runtime_error &e) {
    REQUIRE(std::string(e.what()) == "Content-Length header missing");
  } catch (...) {
    FAIL("Expected std::runtime_error");
  }
}
