#pragma once

#include <string>

namespace jsonrpc {
namespace transport {

/**
 * @brief Base class for framed transport mechanisms.
 *
 * Provides basic functionality for sending and receiving framed messages.
 */
class FramedTransport {
protected:
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

  /**
   * @brief Receives a framed message.
   *
   * Reads headers to determine the content length, then reads the message
   * content based on that length.
   *
   * @param input The input stream to read the framed message.
   * @return The received message.
   */
  std::string DeframeMessage(std::istream &input);

private:
  /**
   * @brief Parses the content length from the header value.
   *
   * @param header_value The header value containing the content length.
   * @return The parsed content length.
   */
  int parseContentLength(const std::string &header_value);
};

} // namespace transport
} // namespace jsonrpc
