#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/transports/stdio_client_transport.hpp"

#include "../common/test_utilities.hpp"

using namespace jsonrpc;

TEST_CASE("StdioClientTransport sends a message correctly",
    "[StdioClientTransport]") {
  std::string test_message = R"({"jsonrpc": "2.0", "method": "example"})";
  std::ostringstream captured_output;
  RedirectIO redirect(std::cin, captured_output);

  StdioClientTransport transport;
  transport.SendRequest(test_message);

  // The tested class adds a newline to the end of the message
  REQUIRE(captured_output.str() == test_message + "\n");
}

TEST_CASE("StdioClientTransport reads a message correctly",
    "[StdioClientTransport]") {
  std::string test_input = R"({"jsonrpc": "2.0", "result": "success"})";

  // Redirect std::cin to simulated input
  std::istringstream simulated_input(test_input + "\n");
  RedirectIO redirect(simulated_input, std::cout);

  StdioClientTransport transport;
  std::string response = transport.ReadResponse();

  REQUIRE(response == test_input);
}

TEST_CASE(
    "StdioClientTransport handles empty message", "[StdioClientTransport]") {
  std::string empty_message = "";

  // Redirect std::cout to capture output
  std::ostringstream captured_output;
  RedirectIO redirect(std::cin, captured_output);

  StdioClientTransport transport;
  transport.SendRequest(empty_message);

  // Even empty messages should end with a newline
  REQUIRE(captured_output.str() == "\n");
}

TEST_CASE(
    "StdioClientTransport handles large message", "[StdioClientTransport]") {
  // Create a large message of 10,000 'x' characters
  std::string large_message(10000, 'x');

  // Redirect std::cout to capture output
  std::ostringstream captured_output;
  RedirectIO redirect(std::cin, captured_output);

  StdioClientTransport transport;
  transport.SendRequest(large_message);

  REQUIRE(captured_output.str() == large_message + "\n");
}

TEST_CASE("StdioClientTransport handles message with special characters",
    "[StdioClientTransport]") {
  std::string special_message =
      R"({"jsonrpc": "2.0", "method": "example", "params": ["newline\n", "tab\t"]})";

  // Redirect std::cout to capture output
  std::ostringstream captured_output;
  RedirectIO redirect(std::cin, captured_output);

  StdioClientTransport transport;
  transport.SendRequest(special_message);

  REQUIRE(captured_output.str() == special_message + "\n");
}

TEST_CASE("StdioClientTransport handles reading empty response",
    "[StdioClientTransport]") {
  std::string empty_input = "";

  // Add newline to simulate input
  std::istringstream simulated_input(empty_input + "\n");
  RedirectIO redirect(simulated_input, std::cout);

  StdioClientTransport transport;
  std::string response = transport.ReadResponse();

  REQUIRE(response == "");
}
