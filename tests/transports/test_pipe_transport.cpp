#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "jsonrpc/transport/pipe_transport.hpp"

#include "../common/test_utils.hpp"

TEST_CASE(
    "PipeTransport starts server and client communication", "[PipeTransport]") {
  std::string socketPath = "/tmp/test_socket";

  // Start the server in a separate thread
  std::thread serverThread([&]() {
    jsonrpc::transport::PipeTransport serverTransport(socketPath, true);

    // Wait for a message from the client
    std::string receivedMessage = serverTransport.ReceiveMessage();
    REQUIRE(receivedMessage == "Hello, Server!");

    // Send a response back to the client
    serverTransport.SendMessage("Hello, Client!");
  });

  // Give the server some time to start
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Start the client and connect to the server
  jsonrpc::transport::PipeTransport clientTransport(socketPath, false);

  // Send a message to the server
  clientTransport.SendMessage("Hello, Server!");

  // Wait for a response from the server
  std::string response = clientTransport.ReceiveMessage();
  REQUIRE(response == "Hello, Client!");

  serverThread.join();
}

TEST_CASE("PipeTransport handles empty message correctly", "[PipeTransport]") {
  std::string socketPath = "/tmp/test_socket_empty";

  // Start the server in a separate thread
  std::thread serverThread([&]() {
    jsonrpc::transport::PipeTransport serverTransport(socketPath, true);

    // Send an empty message to the client
    serverTransport.SendMessage("");
  });

  // Give the server some time to start
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Start the client and connect to the server
  jsonrpc::transport::PipeTransport clientTransport(socketPath, false);

  // Wait for the empty response from the server
  std::string response = clientTransport.ReceiveMessage();
  REQUIRE(response.empty());

  serverThread.join();
}

TEST_CASE("PipeTransport throws on invalid socket path", "[PipeTransport]") {
  REQUIRE_THROWS_WITH(
      jsonrpc::transport::PipeTransport("/tmp/non_existent_socket", false),
      "Error connecting to socket");
}
