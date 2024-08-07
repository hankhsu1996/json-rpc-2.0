#include <sstream>
#include <string>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "jsonrpc/transport/framed_transport.hpp"

using namespace jsonrpc::transport;

namespace jsonrpc {
namespace transport {

class FramedTransportTest : public FramedTransport {
public:
  using FramedTransport::FrameMessage;
  using FramedTransport::ReadContent;
  using FramedTransport::ReadContentLengthFromStream;
  using FramedTransport::ReadHeadersFromStream;
  using FramedTransport::ReceiveFramedMessage;

  int TestParseContentLength(const std::string &header_value) {
    return ParseContentLength(header_value);
  }
};

} // namespace transport
} // namespace jsonrpc

TEST_CASE("FramedTransport correctly frames a message", "[FramedTransport]") {
  FramedTransportTest transport;
  std::ostringstream output;
  std::string message = R"({"jsonrpc":"2.0","method":"testMethod"})";

  transport.FrameMessage(output, message);

  std::string expectedOutput =
      "Content-Length: 39\r\n"
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
      "\r\n" +
      message;

  REQUIRE(output.str() == expectedOutput);
}

TEST_CASE("FramedTransport parses headers correctly", "[FramedTransport]") {
  FramedTransportTest transport;
  std::string headerString =
      "Content-Length: 37\r\nContent-Type: "
      "application/vscode-jsonrpc; charset=utf-8\r\n\r\n";
  std::istringstream headerStream(headerString);

  auto headers = transport.ReadHeadersFromStream(headerStream);

  REQUIRE(headers.size() == 2);
  REQUIRE(headers["Content-Length"] == "37");
  REQUIRE(
      headers["Content-Type"] == "application/vscode-jsonrpc; charset=utf-8");
}

TEST_CASE(
    "FramedTransport returns correct content length", "[FramedTransport]") {
  FramedTransportTest transport;
  std::string headerString =
      "Content-Length: 37\r\nContent-Type: "
      "application/vscode-jsonrpc; charset=utf-8\r\n\r\n";
  std::istringstream headerStream(headerString);

  int contentLength = transport.ReadContentLengthFromStream(headerStream);

  REQUIRE(contentLength == 37);
}

TEST_CASE("FramedTransport reads correct content", "[FramedTransport]") {
  FramedTransportTest transport;
  std::string content = R"({"jsonrpc":"2.0","method":"testMethod"})";
  std::istringstream input(content);

  std::string result = transport.ReadContent(input, content.size());

  REQUIRE(result == content);
}

TEST_CASE(
    "FramedTransport correctly receives framed message", "[FramedTransport]") {
  FramedTransportTest transport;
  std::string framedMessage =
      "Content-Length: 39\r\nContent-Type: application/vscode-jsonrpc; "
      "charset=utf-8\r\n\r\n"
      R"({"jsonrpc":"2.0","method":"testMethod"})";
  std::istringstream input(framedMessage);

  std::string result = transport.ReceiveFramedMessage(input);

  REQUIRE(result == R"({"jsonrpc":"2.0","method":"testMethod"})");
}

TEST_CASE("FramedTransport throws error on invalid content length",
    "[FramedTransport]") {
  FramedTransportTest transport;
  std::istringstream input("Content-Length: invalid\r\n\r\n");

  REQUIRE_THROWS_WITH(transport.ReadContentLengthFromStream(input),
      "Invalid Content-Length value");
}

TEST_CASE("FramedTransport throws error on missing Content-Length",
    "[FramedTransport]") {
  FramedTransportTest transport;
  std::istringstream input(
      "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n\r\n");

  REQUIRE_THROWS_WITH(transport.ReadContentLengthFromStream(input),
      "Content-Length header missing");
}

TEST_CASE("FramedTransport throws error on out of range content length",
    "[FramedTransport]") {
  FramedTransportTest transport;
  std::istringstream input("Content-Length: 9999999999999999999999\r\n\r\n");

  REQUIRE_THROWS_WITH(transport.ReadContentLengthFromStream(input),
      "Content-Length value out of range");
}
