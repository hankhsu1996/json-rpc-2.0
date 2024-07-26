# JSON-RPC 2.0 Modern C++ Library

Welcome to the **JSON-RPC 2.0 Modern C++ Library**! This library provides a lightweight, modern C++ implementation of [JSON-RPC 2.0](https://www.jsonrpc.org/specification) servers and clients. It is designed to be flexible, allowing integration with various transport layers. This library makes it easy to register methods and notifications, binding them to client logic efficiently.

## Table of Contents

- [✨ Features](#-features)
- [🚀 Getting Started](#-getting-started)
  - [Using CMake FetchContent](#using-cmake-fetchcontent)
  - [Using Conan 2](#using-conan-2)
- [📖 Usage and Examples](#-usage-and-examples)
  - [Creating a JSON-RPC Server](#creating-a-json-rpc-server)
  - [Creating a JSON-RPC Client](#creating-a-json-rpc-client)
- [🛠️ Developer Guide](#-developer-guide)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)

## ✨ Features

- **Fully Compliant with JSON-RPC 2.0**: Supports method calls, notifications, comprehensive error handling, and batch requests.
- **Modern C++**: Leverages C++17 features for clean, efficient, and maintainable code.
- **Lightweight**: Designed to focus solely on the JSON-RPC protocol with minimal dependencies.
- **Transport-Agnostic**: The transport layer is abstracted, allowing you to use the provided implementations or define your own.
- **Server and Client Support**: Complete support for both server and client functionalities in JSON-RPC 2.0.
- **Easy Handler Registration**: Handlers can be registered effortlessly using C++ standard functions, lambdas, and more, providing flexibility and ease of use.

## 🚀 Getting Started

To include this library in your project, you can use either CMake's FetchContent module or Conan 2.

### Using CMake FetchContent

Add the following to your `CMakeLists.txt` to fetch the library:

```cmake
include(FetchContent)
FetchContent_Declare(
  jsonrpc
  GIT_REPOSITORY https://github.com/hankhsu1996/json-rpc-2.0.git
  GIT_TAG main
)
FetchContent_MakeAvailable(jsonrpc)
```

### Using Conan 2

Create a `conanfile.txt` in your project directory with the following content:

```ini
[requires]
json-rpc-2.0/1.0.0

[generators]
CMakeDeps
CMakeToolchain

```

You can then install the dependencies using Conan 2 and configure your project with CMake.

## 📖 Usage and Examples

### Creating a JSON-RPC Server

Here’s how to create a simple JSON-RPC server:

```cpp
// Create a server with an HTTP transport
jsonrpc::Server server(std::make_unique<jsonrpc::HttpServerTransport>());

// Register a method named "add" that adds two numbers
server.RegisterMethodCall("add", [](const jsonrpc::Json &params) {
  int result = params[0].get<int>() + params[1].get<int>();
  return {{"result", result}};
});

// Register a notification named "stop" to stop the server
server.RegisterNotification("stop", [&server](const jsonrpc::Json &) {
  server.Stop();
});

// Start the server
server.Start();
```

### Creating a JSON-RPC Client

Here’s how to create a JSON-RPC client:

```cpp
// Create a client with an HTTP transport
auto transport = std::make_unique<jsonrpc::HttpClientTransport>();
jsonrpc::Client client(std::move(transport));

// Perform addition
auto response = client.SendMethodCall("add", {{"a", 10}, {"b", 5}});
std::cout << "Add result: " << response << std::endl;

// Send stop notification
client.SendNotification("stop", {});
```

These examples demonstrate the basic usage of setting up a JSON-RPC server and client. For more examples and detailed usage, please refer to the [examples folder](./examples/).

## 🛠️ Developer Guide

To build and test the project, follow these steps:

Step 1: Install dependencies using Conan:

```bash
conan install . --output-folder=build --build=missing
```

Step 2: Configure and build the project:

```bash
cmake --preset default
cmake --build --preset default
```

Step 3: Run the tests:

```bash
ctest --preset default
```

## 🤝 Contributing

We welcome contributions! If you have suggestions or find any issues, feel free to open an issue or pull request.

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more details.
