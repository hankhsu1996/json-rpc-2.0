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
  server.RegisterMethodCall("add", [&calculator](const nlohmann::json &params) {
    return calculator.Add(params);
  });
  server.RegisterMethodCall(
      "divide", [&calculator](const nlohmann::json &params) {
        return calculator.Divide(params);
      });
  server.RegisterNotification("log",
      [&calculator](const nlohmann::json &params) { calculator.Log(params); });

  server.Start();

  return 0;
}
