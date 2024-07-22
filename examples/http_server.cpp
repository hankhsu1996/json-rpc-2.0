#include <iostream>
#include <memory>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "json_rpc/json_rpc.h"
#include "json_rpc/transports/http_server_transport.h"

#include "calculator.h"

using namespace json_rpc;

void InitializeServerLogger() {
  auto server_logger =
      spdlog::basic_logger_mt("server_logger", "logs/server_logfile.log", true);
  spdlog::set_default_logger(server_logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::debug);
}

void RunServer() {
  InitializeServerLogger();

  auto transport = std::make_unique<HttpServerTransport>("127.0.0.1", 8080);
  Server server(std::move(transport));
  Calculator calculator;

  server.RegisterMethodCall("add",
      [&calculator](const Json &params) { return calculator.Add(params); });

  server.RegisterMethodCall("divide",
      [&calculator](const Json &params) { return calculator.Divide(params); });

  server.RegisterNotification(
      "stop", [&server](const Json &) { server.Stop(); });

  server.Start();
}

int main() {
  RunServer();
  return 0;
}