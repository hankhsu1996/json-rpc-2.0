#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include "json_rpc/client/client.h"
#include "json_rpc/server/server.h"
#include "json_rpc/transports/stdio_transport.h"

#include "calculator.h"

using namespace json_rpc;

void run_server() {
  auto transport = std::make_unique<StdioTransport>();
  Server server(std::move(transport));

  Calculator calculator;
  server.registerMethodCall("add", [&calculator](const nlohmann::json &params) {
    return calculator.add(params);
  });
  server.registerMethodCall(
      "divide", [&calculator](const nlohmann::json &params) {
        return calculator.divide(params);
      });
  server.registerNotification("log",
      [&calculator](const nlohmann::json &params) { calculator.log(params); });

  server.start();
}

void run_client() {
  std::this_thread::sleep_for(
      std::chrono::seconds(1)); // Ensure server starts first

  auto transport = std::make_unique<StdioTransport>();
  Client client(std::move(transport));

  // Perform addition
  nlohmann::json response =
      client.sendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);
  std::cerr << "Client: Received addition result: " << response.dump()
            << std::endl;

  // Log a notification
  client.sendNotification("log", {{"message", "Performed addition"}});

  // Perform division with error handling
  response = client.sendMethodCall("divide", {{"a", 10}, {"b", 0}}, 2);
  std::cerr << "Client: Received division result: " << response.dump()
            << std::endl;

  // Log a notification
  client.sendNotification("log", {{"message", "Attempted division"}});

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
    run_server();
    exit(0);
  } else { // Parent process (Client)
    dup2(pipefd2[0], STDIN_FILENO);
    dup2(pipefd1[1], STDOUT_FILENO);
    close(pipefd1[0]);
    close(pipefd2[1]);
    run_client();
    wait(NULL); // Wait for the child process to finish
  }

  return 0;
}
