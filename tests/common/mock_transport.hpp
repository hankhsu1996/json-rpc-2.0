
#include <queue>
#include <string>
#include <vector>

#include "jsonrpc/transport/transport.hpp"

using namespace jsonrpc::transport;

class MockTransport : public Transport {
public:
  std::vector<std::string> sentRequests;
  std::queue<std::string> responses;

  void SendMessage(const std::string &request) override {
    sentRequests.push_back(request);
  }

  std::string ReceiveMessage() override {
    if (responses.empty()) {
      return "";
    }
    std::string response = responses.front();
    responses.pop();
    return response;
  }

  void SetResponse(const std::string &response) {
    responses.push(response);
  }
};
