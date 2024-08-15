# JSON-RPC 2.0 Modern C++ Library

[![CI](https://github.com/hankhsu1996/json-rpc-2.0/actions/workflows/ci.yml/badge.svg?event=push)](https://github.com/hankhsu1996/json-rpc-2.0/actions/workflows/ci.yml)

Welcome to the **JSON-RPC 2.0 Modern C++ Library**! This library provides a lightweight, modern C++ implementation of [JSON-RPC 2.0](https://www.jsonrpc.org/specification) servers and clients. It is designed to be flexible, allowing integration with various transport layers. This library makes it easy to register methods and notifications, binding them to client logic efficiently.

## ✨ Features

- **Fully Compliant with JSON-RPC 2.0**: Supports method calls, notifications, comprehensive error handling, and batch requests.
- **Modern and Lightweight**: Leverages C++20 features with minimal dependencies, focusing solely on the JSON-RPC protocol.
- **Transport-Agnostic**: Abstract transport layer allows use of provided implementations or custom ones.
- **Simple JSON Integration**: Uses [nlohmann/json](https://github.com/nlohmann/json) for easy JSON object interaction, requiring no learning curve.
- **Flexible Handler Registration**: Register handlers using `std::function`, supporting lambdas, function pointers, and other callable objects.

## 🚀 Getting Started

### Prerequisites

- **Compiler**: Any compiler with C++20 support.
- **Bazel**: Version 7.0+ (for Bzlmod support).
- **CMake**: Version 3.19+ (for CMake preset support, optional).
- **Conan**: Version 2.0+ (optional).

### Using Bazel

To include this library in your project with Bazel, ensure you are using Bazel 7.0 or later, as Bzlmod is enabled by default. Add the following to your `MODULE.bazel` file:

```bazel
bazel_dep(name = "jsonrpc", version = "1.0.0")
```

### Optional: Using CMake FetchContent

If you prefer using CMake, add the library to your project with the following in your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
  jsonrpc
  GIT_REPOSITORY https://github.com/hankhsu1996/json-rpc-2.0.git
  GIT_TAG main
)
FetchContent_MakeAvailable(jsonrpc)
```

### Optional: Using Conan 2

For projects using Conan, create a `conanfile.txt` in your project directory with the following content:

```ini
[requires]
json-rpc-2.0/1.0.0

[generators]
CMakeDeps
CMakeToolchain

```

## 📖 Usage and Examples

### Creating a JSON-RPC Server

Here’s how to create a simple JSON-RPC server:

```cpp
using jsonrpc::server::Server;
using jsonrpc::transport::StdioTransport;
using Json = nlohmann::json;

// Create a server with an stdio transport
Server server(std::make_unique<StdioTransport>());

// Register a method named "add" that adds two numbers
server.RegisterMethodCall("add", [](const std::optional<Json> &params) {
  int result = params.value()["a"] + params.value()["b"];
  return Json{{"result", result}};
});

// Register a notification named "stop" to stop the server
server.RegisterNotification("stop", [&server](const std::optional<Json> &) {
  server.Stop();
});

// Start the server
server.Start();
```

To register a method, you need to provide a function that takes optional `Json` parameters and returns a `Json` object containing either a `result` or `error` field. The `error` field must follow the JSON-RPC spec, including code and message. For simplicity, this library does not provide a more structured way to create error responses.

### Creating a JSON-RPC Client

Here’s how to create a JSON-RPC client:

```cpp
using jsonrpc::client::Client;
using jsonrpc::transport::StdioTransport;
using Json = nlohmann::json;

// Create a client with a standard I/O transport
Client client(std::make_unique<StdioTransport>());
client.Start();

// Perform addition
auto response = client.SendMethodCall("add", Json({{"a", 10}, {"b", 5}}));
spdlog::info("Add result: {}", response.dump());

// Send stop notification
client.SendNotification("stop");
```

These examples demonstrate the basic usage of setting up a JSON-RPC server and client. For more examples and detailed usage, please refer to the [examples folder](./examples/).

## 🛠️ Developer Guide

Follow these steps to build, test, and set up your development environment. Bazel is the preferred method.

### Option 1: Using Bazel (Preferred)

**Step 1: Build the Project**

```
bazel build //...
```

**Step 2: Run Tests**

```
bazel test //...
```

### Option 2: Using CMake and Conan

**Step 1: Install Dependencies**

```
conan profile detect --force
conan install . --build=missing
conan install . -s build_type=Debug --build=missing
```

**Step 2: Configure and Build**

```
cmake --preset release
cmake --build --preset release
```

**Step 3: Run Tests**

```
ctest --preset release
```

### Optional: Debug Configuration

For Debug configuration:

```
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

### Compilation Database

Generate the `compile_commands.json` file for tools like `clang-tidy` and `clangd`:

- **Bazel**: Run `bazel run @hedron_compile_commands//:refresh_all`.
- **CMake**: Simply build the project. The database will be generated automatically.

In both cases, the `compile_commands.json` file will be placed in the root directory.

## 🤝 Contributing

We welcome contributions! If you have suggestions or find any issues, feel free to open an issue or pull request.

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more details.
