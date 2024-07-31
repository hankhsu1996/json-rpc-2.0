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

  auto addReq = std::make_tuple("add", Json({{"a", 10}, {"b", 5}}), false);
  auto addRes = client.SendRequest(addReq);
  spdlog::info("Add result: {}", addRes->dump());

  auto divReq = std::make_tuple("divide", Json({{"a", 10}, {"b", 0}}), false);
  auto divRes = client.SendRequest(divReq);
  spdlog::info("Divide result: {}", divRes->dump());

  auto stopReq = std::make_tuple("stop", std::nullopt, true);
  client.SendRequest(stopReq);

  client.Stop();
  return 0;
}
