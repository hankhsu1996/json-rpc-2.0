#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "jsonrpc/common/transports/framed_transport.hpp"

using namespace jsonrpc::common::transports;

class MockFramedTransport : public FramedTransport {
public:
  using FramedTransport::DeframeMessage;
  using FramedTransport::FrameMessage;
};

TEST_CASE("FramedTransport frames a message correctly", "[FramedTransport]") {
  std::ostringstream output;
  std::string message = R"({"jsonrpc": "2.0", "method": "example"})";

  MockFramedTransport transport;
  transport.FrameMessage(output, message);

  std::string expected_output =
      "Content-Length: 39\r\n"
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n" +
      message;

  REQUIRE(output.str() == expected_output);
}

TEST_CASE("FramedTransport deframes a message correctly", "[FramedTransport]") {
  std::string framed_message =
      "Content-Length: 39\r\n"
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n"
      R"({"jsonrpc": "2.0", "method": "example"})";
  std::istringstream input(framed_message);

  MockFramedTransport transport;
  std::string message = transport.DeframeMessage(input);

  REQUIRE(message == R"({"jsonrpc": "2.0", "method": "example"})");
}

TEST_CASE("FramedTransport throws when Content-Length header is missing",
    "[FramedTransport]") {
  std::string framed_message =
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n"
      R"({"jsonrpc": "2.0", "method": "example"})";
  std::istringstream input(framed_message);

  MockFramedTransport transport;
  REQUIRE_THROWS_WITH(
      transport.DeframeMessage(input), "Content-Length header missing");
}

TEST_CASE("FramedTransport throws on invalid Content-Length value",
    "[FramedTransport]") {
  std::string framed_message =
      "Content-Length: abc\r\n"
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n"
      R"({"jsonrpc": "2.0", "method": "example"})";
  std::istringstream input(framed_message);

  MockFramedTransport transport;
  REQUIRE_THROWS_WITH(
      transport.DeframeMessage(input), "Invalid Content-Length value");
}

TEST_CASE("FramedTransport throws when content read is incomplete",
    "[FramedTransport]") {
  std::string framed_message =
      "Content-Length: 50\r\n"
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n"
      R"({"jsonrpc": "2.0", "method": "example"})";
  std::istringstream input(framed_message);

  MockFramedTransport transport;
  REQUIRE_THROWS_WITH(transport.DeframeMessage(input),
      "Failed to read the expected content length");
}
