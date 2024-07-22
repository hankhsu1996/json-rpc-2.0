#pragma once

#include <string>

#include "json_rpc/core/types.h"

namespace json_rpc {

class FramedTransport {
protected:
  void SendMessage(const std::string &message);
  std::string ReceiveMessage();

private:
  int parseContentLength(const std::string &header_value);
  void readHeaders(int &content_length);
};

} // namespace json_rpc
