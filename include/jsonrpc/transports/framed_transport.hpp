#pragma once

#include <string>

namespace jsonrpc {

class FramedTransport {
protected:
  void FrameMessage(std::ostream &output, const std::string &message);
  std::string DeframeMessage(std::istream &input);

private:
  int parseContentLength(const std::string &header_value);
};

} // namespace jsonrpc
