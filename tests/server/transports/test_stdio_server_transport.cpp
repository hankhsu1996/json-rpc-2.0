#include <iostream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include "jsonrpc/server/dispatcher.hpp"
#include "jsonrpc/server/transports/stdio_server_transport.hpp"

#include "../../common/test_utilities.hpp"

using namespace jsonrpc::server;
using namespace jsonrpc::server::transports;

class MockDispatcher : public IDispatcher {
public:
  std::optional<std::string> DispatchRequest(
      const std::string &request) override {
    return R"({"jsonrpc": "2.0", "result": "success", "id": 1})";
  }
};

TEST_CASE("StdioServerTransport listens and dispatches requests correctly",
    "[StdioServerTransport]") {
  // Simulate input for the server
  std::string simulated_input =
      R"({"jsonrpc": "2.0", "method": "testMethod", "id": 1})" +
      std::string("\n");
  std::istringstream input_stream(simulated_input);

  // Capture output from the server
  std::ostringstream output_stream;
  RedirectIO redirect(input_stream, output_stream);

  // Set up the StdioServerTransport with a mock dispatcher
  auto mock_dispatcher = std::make_shared<MockDispatcher>();
  StdioServerTransport transport;
  transport.SetDispatcher(mock_dispatcher);

  // Start the server (runs Listen in the main thread for testing)
  transport.Start();

  // Verify the response
  std::string expected_output =
      R"({"jsonrpc": "2.0", "result": "success", "id": 1})" + std::string("\n");
  REQUIRE(output_stream.str() == expected_output);
}

TEST_CASE(
    "StdioServerTransport stops listening on EOF", "[StdioServerTransport]") {
  // Simulate EOF by providing no input (or an empty string with an EOF
  // condition)
  std::istringstream input_stream("");

  // Capture output from the server
  std::ostringstream output_stream;
  RedirectIO redirect(input_stream, output_stream);

  // Set up the StdioServerTransport with a mock dispatcher
  auto mock_dispatcher = std::make_shared<MockDispatcher>();
  StdioServerTransport transport;
  transport.SetDispatcher(mock_dispatcher);

  // Start the server (runs Listen in the main thread for testing)
  transport.Start();

  // Expect no output as there's no valid input to process
  REQUIRE(output_stream.str().empty());
}
