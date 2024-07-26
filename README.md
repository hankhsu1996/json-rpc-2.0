# JSON-RPC 2.0 Modern C++ Library

Welcome to the **JSON-RPC 2.0 Modern C++ Library**! This library provides a lightweight, modern C++ implementation of [JSON-RPC 2.0](https://www.jsonrpc.org/specification) servers and clients. It is designed to be flexible, allowing integration with various transport layers. This library makes it easy to register methods and notifications, binding them to client logic efficiently.

## ✨ Features

- **Fully Compliant with JSON-RPC 2.0**: Supports method calls, notifications, comprehensive error handling, and batch requests.
- **Modern and Lightweight**: Leverages C++17 features with minimal dependencies, focusing solely on the JSON-RPC protocol.
- **Transport-Agnostic**: Abstract transport layer allows use of provided implementations or custom ones.
- **Simple JSON Integration**: Uses [nlohmann/json](https://github.com/nlohmann/json) for easy JSON object interaction, requiring no learning curve.
- **Flexible Handler Registration**: Register handlers using `std::function`, supporting lambdas, function pointers, and other callable objects.

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
server.RegisterMethodCall("add", [](const std::optional<jsonrpc::Json> &params) {
  int a = params.value()["a"];
  int b = params.value()["b"];
  return Json{{"result", a + b}};
});

// Register a notification named "stop" to stop the server
server.RegisterNotification("stop", [&server](const std::optional<jsonrpc::Json> &) {
  server.Stop();
});

// Start the server
server.Start();
```

To register a method, you need to provide a function that takes optional `Json` parameters and returns a `Json` object containing either a `result` or `error` field. The `error` field must follow the JSON-RPC spec, including code and message. For simplicity, this library does not provide a more structured way to create error responses.

### Creating a JSON-RPC Client

Here’s how to create a JSON-RPC client:

```cpp
// Create a client with an HTTP transport
auto transport = std::make_unique<jsonrpc::HttpClientTransport>();
jsonrpc::Client client(std::move(transport));

// Perform addition
auto response = client.SendMethodCall("add", {{"a", 10}, {"b", 5}});
std::cout << "Add result: " << response.dump() << std::endl;

// Send stop notification
client.SendNotification("stop", {});
```

These examples demonstrate the basic usage of setting up a JSON-RPC server and client. For more examples and detailed usage, please refer to the [examples folder](./examples/).

## 🛠️ Developer Guide

To build and test the project, follow these steps:

### Step 1: Install Dependencies

Run Conan to install the dependencies specified in the `conanfile.txt`:

```bash
conan install . --output-folder=build --build=missing
```

### Step 2: Configure and Build the Project

Use CMake presets for configuration and building. Ensure you have CMake 3.19 or above to utilize the modern preset feature. The configuration and build commands use `CMakePresets.json`:

```bash
cmake --preset default
cmake --build --preset default
```

### Step 3: Run Tests

Run the tests using CMake presets, as defined in `CMakePresets.json`:

```bash
ctest --preset default
```

All commands should be executed from the top-level project directory. There is no need to navigate to the build directory.

## 🤝 Contributing

We welcome contributions! If you have suggestions or find any issues, feel free to open an issue or pull request.

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more details.
