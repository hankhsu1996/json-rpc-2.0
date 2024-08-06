# JSON-RPC 2.0 Modern C++ Library

[![CI](https://github.com/hankhsu1996/json-rpc-2.0/actions/workflows/ci.yml/badge.svg?event=push)](https://github.com/hankhsu1996/json-rpc-2.0/actions/workflows/ci.yml)

Welcome to the **JSON-RPC 2.0 Modern C++ Library**! This library provides a lightweight, modern C++ implementation of [JSON-RPC 2.0](https://www.jsonrpc.org/specification) servers and clients. It is designed to be flexible, allowing integration with various transport layers. This library makes it easy to register methods and notifications, binding them to client logic efficiently.

## ✨ Features

- **Fully Compliant with JSON-RPC 2.0**: Supports method calls, notifications, comprehensive error handling, and batch requests.
- **Modern and Lightweight**: Leverages C++17 features with minimal dependencies, focusing solely on the JSON-RPC protocol.
- **Transport-Agnostic**: Abstract transport layer allows use of provided implementations or custom ones.
- **Simple JSON Integration**: Uses [nlohmann/json](https://github.com/nlohmann/json) for easy JSON object interaction, requiring no learning curve.
- **Flexible Handler Registration**: Register handlers using `std::function`, supporting lambdas, function pointers, and other callable objects.

## 🚀 Getting Started

To include this library in your project, you can use CMake's FetchContent, Conan 2, or Bazel.

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

### Using Bazel

Add the following to your `MODULE.bazel` file:

```bazel
bazel_dep(name = "jsonrpc", version = "1.0.0")
```

## 📖 Usage and Examples

### Creating a JSON-RPC Server

Here’s how to create a simple JSON-RPC server:

```cpp
using namespace jsonrpc;
using Json = nlohmann::json;

// Create a server with an stdio transport
server::Server server(std::make_unique<transport::StdioTransport>());

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
using namespace jsonrpc;
using Json = nlohmann::json;

// Create a client with an stdio transport
client::Client client(std::make_unique<transport::StdioTransport>());
client.Start();

// Perform addition
auto response = client.SendMethodCall("add", Json({{"a", 10}, {"b", 5}}));
std::cout << "Add result: " << response.dump() << std::endl;

// Send stop notification
client.SendNotification("stop");
```

These examples demonstrate the basic usage of setting up a JSON-RPC server and client. For more examples and detailed usage, please refer to the [examples folder](./examples/).

## 🛠️ Developer Guide

To build and test the project, follow these steps. Bazel is the preferred method.

### Option 1: Using Bazel (Preferred)

**Step 1: Build the Project**

Simply run:

```bash
bazel build //...
```

**Step 2: Run Tests**

To run all tests:

```bash
bazel test //...
```

### Option 2: Using CMake and Conan

**Step 1: Install Dependencies**

Ensure you have a Conan profile configured. If the default profile (`.conan2/profiles/default`) is missing, create it:

```bash
conan profile detect --force
```

Next, install dependencies and generate `ConanPresets.json`:

```bash
conan install . --build=missing
```

**Step 2: Configure and Build the Project**

Use CMake presets to configure and build the project. Ensure CMake 3.19+ is installed.

For Release configuration:

```bash
cmake --preset release
cmake --build --preset release
```

**Step 3: Run Tests**

Run tests using the appropriate CMake preset:

```bash
ctest --preset release
```

### Optional: Debug Configuration

For Debug configuration:

```bash
conan install . -s build_type=Debug --build=missing
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

## 🤝 Contributing

We welcome contributions! If you have suggestions or find any issues, feel free to open an issue or pull request.

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more details.
