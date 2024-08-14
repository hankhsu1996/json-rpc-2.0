#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "jsonrpc/transport/framed_transport.hpp"

namespace jsonrpc::transport {

class FramedTransportTest : public jsonrpc::transport::FramedTransport {
 public:
  using jsonrpc::transport::FramedTransport::FrameMessage;
  using jsonrpc::transport::FramedTransport::ReadContent;
  using jsonrpc::transport::FramedTransport::ReadContentLengthFromStream;
  using jsonrpc::transport::FramedTransport::ReadHeadersFromStream;
  using jsonrpc::transport::FramedTransport::ReceiveFramedMessage;

  static auto TestParseContentLength(const std::string &header_value) -> int {
    return ParseContentLength(header_value);
  }
};

}  // namespace jsonrpc::transport

TEST_CASE("FramedTransport correctly frames a message", "[FramedTransport]") {
  std::ostringstream output;
  std::string message = R"({"jsonrpc":"2.0","method":"testMethod"})";

  jsonrpc::transport::FramedTransportTest::FrameMessage(output, message);

  std::string expected_output =
      "Content-Length: 39\r\n"
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n" +
      message;

  REQUIRE(output.str() == expected_output);
}

TEST_CASE("FramedTransport parses headers correctly", "[FramedTransport]") {
  std::string header_string =
      "Content-Length: 37\r\nContent-Type: "
      "application/vscode-jsonrpc; charset=utf-8\r\n\r\n";
  std::istringstream header_stream(header_string);

  auto headers = jsonrpc::transport::FramedTransportTest::ReadHeadersFromStream(
      header_stream);

  REQUIRE(headers.size() == 2);
  REQUIRE(headers["Content-Length"] == "37");
  REQUIRE(
      headers["Content-Type"] == "application/vscode-jsonrpc; charset=utf-8");
}

TEST_CASE(
    "FramedTransport returns correct content length", "[FramedTransport]") {
  std::string header_string =
      "Content-Length: 37\r\nContent-Type: "
      "application/vscode-jsonrpc; charset=utf-8\r\n\r\n";
  std::istringstream header_stream(header_string);

  int content_length =
      jsonrpc::transport::FramedTransportTest::ReadContentLengthFromStream(
          header_stream);

  REQUIRE(content_length == 37);
}

TEST_CASE("FramedTransport reads correct content", "[FramedTransport]") {
  std::string content = R"({"jsonrpc":"2.0","method":"testMethod"})";
  std::istringstream input(content);

  std::string result = jsonrpc::transport::FramedTransportTest::ReadContent(
      input, content.size());

  REQUIRE(result == content);
}

TEST_CASE(
    "FramedTransport correctly receives framed message", "[FramedTransport]") {
  std::string framed_message =
      "Content-Length: 39\r\nContent-Type: application/vscode-jsonrpc; "
      "charset=utf-8\r\n\r\n"
      R"({"jsonrpc":"2.0","method":"testMethod"})";
  std::istringstream input(framed_message);

  std::string result =
      jsonrpc::transport::FramedTransportTest::ReceiveFramedMessage(input);

  REQUIRE(result == R"({"jsonrpc":"2.0","method":"testMethod"})");
}

TEST_CASE(
    "FramedTransport throws error on invalid content length",
    "[FramedTransport]") {
  jsonrpc::transport::FramedTransportTest transport;
  std::istringstream input("Content-Length: invalid\r\n\r\n");

  REQUIRE_THROWS_WITH(
      transport.ReadContentLengthFromStream(input),
      "Invalid Content-Length value");
}

TEST_CASE(
    "FramedTransport throws error on missing Content-Length",
    "[FramedTransport]") {
  jsonrpc::transport::FramedTransportTest transport;
  std::istringstream input(
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n\r\n");

  REQUIRE_THROWS_WITH(
      transport.ReadContentLengthFromStream(input),
      "Content-Length header missing");
}

TEST_CASE(
    "FramedTransport throws error on out of range content length",
    "[FramedTransport]") {
  jsonrpc::transport::FramedTransportTest transport;
  std::istringstream input("Content-Length: 9999999999999999999999\r\n\r\n");

  REQUIRE_THROWS_WITH(
      transport.ReadContentLengthFromStream(input),
      "Content-Length value out of range");
}
