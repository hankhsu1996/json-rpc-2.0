#include <iostream>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "json_rpc/client/client.h"
#include "json_rpc/transports/stdio_transport.h"

using namespace json_rpc;

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Client client(std::move(transport));

  // Perform addition
  nlohmann::json response =
      client.sendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);
  std::cout << "Addition result: " << response.dump() << std::endl;

  // Log a notification
  client.sendNotification("log", {{"message", "Performed addition"}});

  // Perform division with error handling
  response = client.sendMethodCall("divide", {{"a", 10}, {"b", 0}}, 2);
  std::cout << "Division result: " << response.dump() << std::endl;

  // Log a notification
  client.sendNotification("log", {{"message", "Attempted division"}});

  return 0;
}
