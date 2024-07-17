#include <iostream>
#include <memory>

#include <nlohmann/json.hpp>

#include "json_rpc/json_rpc.h"

using namespace json_rpc;
using Json = nlohmann::json;

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Client client(std::move(transport));

  // Perform addition
  Json response = client.SendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);
  std::cout << "Addition result: " << response.dump() << std::endl;

  // Log a notification
  client.SendNotification("log", {{"message", "Performed addition"}});

  // Perform division with error handling
  response = client.SendMethodCall("divide", {{"a", 10}, {"b", 0}}, 2);
  std::cout << "Division result: " << response.dump() << std::endl;

  // Log a notification
  client.SendNotification("log", {{"message", "Attempted division"}});

  return 0;
}
