#pragma once

#include <string>

#include "json_rpc/core/types.hpp"

namespace json_rpc {

/**
 * @brief Base class for framed transport mechanisms.
 *
 * Provides basic functionality for sending and receiving framed messages.
 */
class FramedTransport {
protected:
  /**
   * @brief Sends a framed message.
   *
   * Sends a JSON message as a string with additional headers for Content-Length
   * and Content-Type, similar to HTTP.
   *
   * @param message The message to be sent.
   */
  void SendMessage(const std::string &message);

  /**
   * @brief Receives a framed message.
   *
   * Reads headers to determine the content length, then reads the message
   * content based on that length.
   *
   * @return The received message.
   */
  std::string ReceiveMessage();

private:
  /**
   * @brief Parses the content length from the header value.
   *
   * @param header_value The header value containing the content length.
   * @return The parsed content length.
   */
  int parseContentLength(const std::string &header_value);

  /**
   * @brief Reads headers to determine the content length.
   *
   * Internally calls `parseContentLength` to extract the content length.
   *
   * @param content_length A reference to store the parsed content length.
   */
  void readHeaders(int &content_length);
};

} // namespace json_rpc
