#include <iostream>
#include <memory>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "jsonrpc/jsonrpc.hpp"
#include "jsonrpc/transports/http_client_transport.hpp"

using namespace jsonrpc;

void InitializeClientLogger() {
  auto client_logger =
      spdlog::basic_logger_mt("client_logger", "logs/client_logfile.log", true);
  spdlog::set_default_logger(client_logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::debug);
}

void RunClient() {
  InitializeClientLogger();

  std::string url = "http://127.0.0.1:8080";
  auto transport = std::make_unique<HttpClientTransport>(url);
  Client client(std::move(transport));

  // Perform addition
  Json response = client.SendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);
  spdlog::info("Client received add result: {}", response.dump());

  // Perform division
  response = client.SendMethodCall("divide", {{"a", 10}, {"b", 0}}, 2);
  spdlog::info("Client received divide result: {}", response.dump());

  // Send stop notification
  client.SendNotification("stop", {});
}

int main() {
  RunClient();
  return 0;
}
