#include <memory>

#include <jsonrpc/server/server.hpp>
#include <jsonrpc/server/transports/stdio_server_transport.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "calculator.hpp"

using namespace jsonrpc::server;
using Json = nlohmann::json;

int main() {
  auto logger = spdlog::basic_logger_mt("server_logger", "logs/server.log");
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::debug);

  auto transport = std::make_unique<transports::StdioServerTransport>();
  Server server(std::move(transport));
  Calculator calculator;

  server.RegisterMethodCall(
      "add", [&calculator](const std::optional<Json> &params) {
        return calculator.Add(params.value());
      });

  server.RegisterMethodCall(
      "divide", [&calculator](const std::optional<Json> &params) {
        return calculator.Divide(params.value());
      });

  server.RegisterNotification("stop", [&server](const std::optional<Json> &) {
    spdlog::info("Server Received stop notification");
    server.Stop();
  });

  server.Start();
  return 0;
}
