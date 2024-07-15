#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include "json_rpc/core/error.h"
#include "json_rpc/core/request.h"
#include "json_rpc/core/response.h"
#include "json_rpc/server/server.h"
#include "json_rpc/transports/stdio_transport.h"

#include "calculator.h"

using namespace json_rpc;

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Server server(std::move(transport));

  Calculator calculator;
  server.registerMethod("add", [&calculator](const Request &request) {
    return calculator.add(request);
  });
  server.registerMethod("divide", [&calculator](const Request &request) {
    return calculator.divide(request);
  });
  server.registerNotification("log",
      [&calculator](const Request &request) { calculator.log(request); });

  server.start();

  return 0;
}
