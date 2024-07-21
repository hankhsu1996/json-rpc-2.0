#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "json_rpc/json_rpc.h"

#include "calculator.h"

using namespace json_rpc;
using Json = nlohmann::json;

void InitializeServerLogger() {
  auto server_logger =
      spdlog::basic_logger_mt("server_logger", "logs/server_logfile.log", true);
  spdlog::set_default_logger(server_logger);
  spdlog::set_level(spdlog::level::debug);
}

void InitializeClientLogger() {
  auto client_logger =
      spdlog::basic_logger_mt("client_logger", "logs/client_logfile.log", true);
  spdlog::set_default_logger(client_logger);
  spdlog::set_level(spdlog::level::debug);
}

void RunServer() {
  InitializeServerLogger();

  auto transport = std::make_unique<StdioServerTransport>();
  Server server(std::move(transport));
  Calculator calculator;

  server.RegisterMethodCall("add",
      [&calculator](const Json &params) { return calculator.Add(params); });

  server.RegisterMethodCall("divide",
      [&calculator](const Json &params) { return calculator.Divide(params); });

  server.RegisterNotification(
      "log", [&calculator](const Json &params) { calculator.Log(params); });

  // Register a stop notification
  server.RegisterNotification("stop", [&server](const Json &) {
    spdlog::info("Server Received stop notification");
    server.Stop();
  });

  server.Start();
}

void RunClient() {
  InitializeClientLogger();

  std::this_thread::sleep_for(
      std::chrono::seconds(1)); // Ensure server starts first

  auto transport = std::make_unique<StdioClientTransport>();
  Client client(std::move(transport));

  // Perform addition
  Json response = client.SendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);
  spdlog::info(
      "Client: Received addition result: {}", response["result"].dump());

  // Log a notification
  client.SendNotification("log", {{"message", "Performed addition"}});

  // Perform division with error handling

  response = client.SendMethodCall("divide", {{"a", 10}, {"b", 0}}, 2);
  if (response.contains("error")) {
    spdlog::info("Client: Received division error: {}",
        response["error"]["message"].dump());
  } else {
    spdlog::info(
        "Client: Received division result: {}", response["result"].dump());
  }

  // Log a notification
  client.SendNotification("log", {{"message", "Attempted division"}});

  // Send stop notification
  client.SendNotification("stop", {});

  // Wait for a short duration before exiting
  std::this_thread::sleep_for(std::chrono::seconds(1));
  spdlog::info("Example finished");
}

int main() {
  int pipefd1[2];
  int pipefd2[2];

  if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) { // Child process (Server)
    dup2(pipefd1[0], STDIN_FILENO);
    dup2(pipefd2[1], STDOUT_FILENO);
    close(pipefd1[1]);
    close(pipefd2[0]);
    RunServer();
    exit(0);
  } else { // Parent process (Client)
    dup2(pipefd2[0], STDIN_FILENO);
    dup2(pipefd1[1], STDOUT_FILENO);
    close(pipefd1[0]);
    close(pipefd2[1]);
    RunClient();
    wait(NULL); // Wait for the child process to finish
  }

  return 0;
}
