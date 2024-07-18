#pragma once

#include <string>

#include "json_rpc/core/transport.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class FramedTransport {
protected:
  void SendMessage(const std::string &message);
  std::string ReceiveMessage();
};

} // namespace json_rpc
