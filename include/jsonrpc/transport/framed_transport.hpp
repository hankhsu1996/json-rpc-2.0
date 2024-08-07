#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <unordered_map>

namespace jsonrpc {
namespace transport {

class FramedTransportTest;

/**
 * @brief Base class for framed transport mechanisms.
 *
 * Provides modular functionality for sending and receiving framed messages.
 */
class FramedTransport {
  /// @brief A map of headers to their values.
  using HeaderMap = std::unordered_map<std::string, std::string>;

protected:
  /// @brief The delimiter used to separate headers from the message content.
  static constexpr const char *HEADER_DELIMITER = "\r\n\r\n";

  /**
   * @brief Constructs a framed message.
   *
   * Constructs a JSON message as a string with additional headers for
   * Content-Length and Content-Type, similar to HTTP.
   *
   * @param output The output stream to write the framed message.
   * @param message The message to be framed.
   */
  void FrameMessage(std::ostream &output, const std::string &message);

  HeaderMap ReadHeadersFromStream(std::istream &input);
  int ReadContentLengthFromStream(std::istream &input);

  /**
   * @brief Reads content from the input stream based on the content length.
   *
   * @param input The input stream to read the content from.
   * @param content_length The length of the content to be read.
   * @return The content as a string.
   */
  std::string ReadContent(std::istream &input, int content_length);

  /**
   * @brief Receives a framed message.
   *
   * Reads headers to determine the content length, then reads the message
   * content based on that length.
   *
   * @param input The input stream to read the framed message.
   * @return The received message content.
   */
  std::string ReceiveFramedMessage(std::istream &input);

private:
  /**
   * @brief Parses the content length from the header value.
   *
   * @param header_value The header value containing the content length.
   * @return The parsed content length.
   */
  int ParseContentLength(const std::string &header_value);

  friend class FramedTransportTest;
};

} // namespace transport
} // namespace jsonrpc
