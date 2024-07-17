#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include "json_rpc/json_rpc.h"

#include "calculator.h"

using namespace json_rpc;
using Json = nlohmann::json;

void RunServer() {
  auto transport = std::make_unique<StdioTransport>();
  Server server(std::move(transport));
  Calculator calculator;

  server.RegisterMethodCall("add",
      [&calculator](const Json &params) { return calculator.Add(params); });

  server.RegisterMethodCall("divide",
      [&calculator](const Json &params) { return calculator.Divide(params); });

  server.RegisterNotification(
      "log", [&calculator](const Json &params) { calculator.Log(params); });

  server.Start();
}

void RunClient() {
  std::this_thread::sleep_for(
      std::chrono::seconds(1)); // Ensure server starts first

  auto transport = std::make_unique<StdioTransport>();
  Client client(std::move(transport));

  // Perform addition
  Json response = client.SendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);
  std::cerr << "Client: Received addition result: " << response.dump()
            << std::endl;

  // Log a notification
  client.SendNotification("log", {{"message", "Performed addition"}});

  // Perform division with error handling
  response = client.SendMethodCall("divide", {{"a", 10}, {"b", 0}}, 2);
  std::cerr << "Client: Received division result: " << response.dump()
            << std::endl;

  // Log a notification
  client.SendNotification("log", {{"message", "Attempted division"}});

  // Wait for a short duration before stopping the server
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cerr << "Client: Example finished. Press Ctrl+C to terminate the server."
            << std::endl;
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
