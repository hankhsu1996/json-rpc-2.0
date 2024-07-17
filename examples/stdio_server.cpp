#include <memory>

#include <nlohmann/json.hpp>

#include "json_rpc/json_rpc.h"

#include "calculator.h"

using namespace json_rpc;
using Json = nlohmann::json;

int main() {
  auto transport = std::make_unique<StdioServerTransport>();
  Server server(std::move(transport));
  Calculator calculator;

  server.RegisterMethodCall("add",
      [&calculator](const Json &params) { return calculator.Add(params); });

  server.RegisterMethodCall("divide",
      [&calculator](const Json &params) { return calculator.Divide(params); });

  server.RegisterNotification(
      "log", [&calculator](const Json &params) { calculator.Log(params); });

  server.Start();

  return 0;
}
