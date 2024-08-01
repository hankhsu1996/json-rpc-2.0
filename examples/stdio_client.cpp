#include <iostream>
#include <memory>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "jsonrpc/jsonrpc.hpp"

using namespace jsonrpc;

int main() {
  auto logger = spdlog::basic_logger_mt("client_logger", "logs/client.log");
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::debug);

  auto transport = std::make_unique<StdioClientTransport>();
  Client client(std::move(transport));
  client.Start();

  Json addRes = client.SendMethodCall("add", Json({{"a", 10}, {"b", 5}}));
  spdlog::info("Add result: {}", addRes.dump());

  Json divRes = client.SendMethodCall("divide", Json({{"a", 10}, {"b", 0}}));
  spdlog::info("Divide result: {}", divRes.dump());

  client.SendNotification("stop");

  client.Stop();
  return 0;
}
