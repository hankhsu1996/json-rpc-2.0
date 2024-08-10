#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "jsonrpc/transport/pipe_transport.hpp"

TEST_CASE(
    "PipeTransport starts server and client communication", "[PipeTransport]") {
  std::string socket_path = "/tmp/test_socket";

  // Start the server in a separate thread
  std::thread server_thread([&]() {
    jsonrpc::transport::PipeTransport server_transport(socket_path, true);

    // Wait for a message from the client
    std::string received_message = server_transport.ReceiveMessage();
    REQUIRE(received_message == "Hello, Server!");

    // Send a response back to the client
    server_transport.SendMessage("Hello, Client!");
  });

  // Give the server some time to start
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Start the client and connect to the server
  jsonrpc::transport::PipeTransport client_transport(socket_path, false);

  // Send a message to the server
  client_transport.SendMessage("Hello, Server!");

  // Wait for a response from the server
  std::string response = client_transport.ReceiveMessage();
  REQUIRE(response == "Hello, Client!");

  server_thread.join();
}

TEST_CASE("PipeTransport handles empty message correctly", "[PipeTransport]") {
  std::string socket_path = "/tmp/test_socket_empty";

  // Start the server in a separate thread
  std::thread server_thread([&]() {
    jsonrpc::transport::PipeTransport server_transport(socket_path, true);

    // Send an empty message to the client
    server_transport.SendMessage("");
  });

  // Give the server some time to start
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Start the client and connect to the server
  jsonrpc::transport::PipeTransport client_transport(socket_path, false);

  // Wait for the empty response from the server
  std::string response = client_transport.ReceiveMessage();
  REQUIRE(response.empty());

  server_thread.join();
}

TEST_CASE("PipeTransport throws on invalid socket path", "[PipeTransport]") {
  REQUIRE_THROWS_WITH(
      jsonrpc::transport::PipeTransport("/tmp/non_existent_socket", false),
      "Error connecting to socket");
}
