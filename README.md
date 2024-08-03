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
// Create a server with an stdio transport
jsonrpc::Server server(std::make_unique<jsonrpc::StdioServerTransport>());

// Register a method named "add" that adds two numbers
server.RegisterMethodCall("add", [](const std::optional<jsonrpc::Json> &params) {
  int result = params.value()["a"] + params.value()["b"];
  return Json{{"result", result}};
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
// Create a client with an stdio transport
auto transport = std::make_unique<jsonrpc::StdioClientTransport>();
jsonrpc::Client client(std::move(transport));
client.Start();

// Perform addition
auto response = client.SendMethodCall("add", Json({{"a", 10}, {"b", 5}}));
std::cout << "Add result: " << response.dump() << std::endl;

// Send stop notification
client.SendNotification("stop");
```

These examples demonstrate the basic usage of setting up a JSON-RPC server and client. For more examples and detailed usage, please refer to the [examples folder](./examples/).

## 🛠️ Developer Guide

To build and test the project, follow these steps:

### Step 1: Install Dependencies

Before installing dependencies, ensure that you have a Conan profile configured. If the default profile (`.conan2/profiles/default`) does not exist, generate one based on your current machine setup:

```bash
conan profile detect --force
```

This command will create a new default profile or overwrite the existing one if needed.

Next, install the required dependencies using Conan. This step will also generate the necessary `ConanPresets.json` for CMake:

```bash
conan install . --build=missing
```

### Step 2: Configure and Build the Project

Next, use CMake presets to configure and build the project. Ensure you have CMake 3.19 or above to leverage the preset feature. The following commands will use the presets defined in `CMakePresets.json`:

For Release configuration:

```bash
cmake --preset release
cmake --build --preset release
```

### Step 3: Run Tests

After building, run the tests using the appropriate CMake preset:

```bash
ctest --preset release
```

### Optional: Debug Configuration

If you need to work with the Debug configuration, you can follow these steps:

```bash
conan install . -s build_type=Debug
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

### Additional Notes

- All commands should be executed from the top-level project directory.
- There is no need to navigate to the `build` directory, as the presets handle the configuration and build paths automatically.

## 🤝 Contributing

We welcome contributions! If you have suggestions or find any issues, feel free to open an issue or pull request.

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more details.
