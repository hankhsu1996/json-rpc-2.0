#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/transport/stdio_transport.hpp"

#include "../common/test_utils.hpp"

using namespace jsonrpc::transport;

TEST_CASE("StdioTransport sends a message correctly", "[StdioTransport]") {
  std::string test_message = R"({"jsonrpc": "2.0", "method": "example"})";
  std::ostringstream captured_output;
  RedirectIO redirect(std::cin, captured_output);

  StdioTransport transport;
  transport.SendMessage(test_message);

  // The tested class adds a newline to the end of the message
  REQUIRE(captured_output.str() == test_message + "\n");
}

TEST_CASE("StdioTransport reads a message correctly", "[StdioTransport]") {
  std::string test_input = R"({"jsonrpc": "2.0", "result": "success"})";

  // Redirect std::cin to simulated input
  std::istringstream simulated_input(test_input + "\n");
  RedirectIO redirect(simulated_input, std::cout);

  StdioTransport transport;
  std::string response = transport.ReceiveMessage();

  REQUIRE(response == test_input);
}

TEST_CASE("StdioTransport handles empty message", "[StdioTransport]") {
  std::string empty_message = "";

  // Redirect std::cout to capture output
  std::ostringstream captured_output;
  RedirectIO redirect(std::cin, captured_output);

  StdioTransport transport;
  transport.SendMessage(empty_message);

  // Even empty messages should end with a newline
  REQUIRE(captured_output.str() == "\n");
}

TEST_CASE("StdioTransport handles large message", "[StdioTransport]") {
  // Create a large message of 10,000 'x' characters
  std::string large_message(10000, 'x');

  // Redirect std::cout to capture output
  std::ostringstream captured_output;
  RedirectIO redirect(std::cin, captured_output);

  StdioTransport transport;
  transport.SendMessage(large_message);

  REQUIRE(captured_output.str() == large_message + "\n");
}

TEST_CASE("StdioTransport handles message with special characters",
    "[StdioTransport]") {
  std::string special_message =
      R"({"jsonrpc": "2.0", "method": "example", "params": ["newline\n", "tab\t"]})";

  // Redirect std::cout to capture output
  std::ostringstream captured_output;
  RedirectIO redirect(std::cin, captured_output);

  StdioTransport transport;
  transport.SendMessage(special_message);

  REQUIRE(captured_output.str() == special_message + "\n");
}

TEST_CASE("StdioTransport handles reading empty response", "[StdioTransport]") {
  std::string empty_input = "";

  // Add newline to simulate input
  std::istringstream simulated_input(empty_input + "\n");
  RedirectIO redirect(simulated_input, std::cout);

  StdioTransport transport;
  std::string response = transport.ReceiveMessage();

  REQUIRE(response == "");
}
