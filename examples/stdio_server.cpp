#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include "json_rpc/server/server.h"
#include "json_rpc/transports/stdio_transport.h"

#include "calculator.h"

using namespace json_rpc;

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Server server(std::move(transport));

  Calculator calculator;
  server.registerMethodCall("add", [&calculator](const nlohmann::json &params) {
    return calculator.add(params);
  });
  server.registerMethodCall(
      "divide", [&calculator](const nlohmann::json &params) {
        return calculator.divide(params);
      });
  server.registerNotification("log",
      [&calculator](const nlohmann::json &params) { calculator.log(params); });

  server.start();

  return 0;
}
