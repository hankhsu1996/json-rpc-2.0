# JSON-RPC 2.0 C++ Implementation

This project is a lightweight, modern C++ library for implementing JSON-RPC 2.0 servers and clients. It is designed to be flexible, allowing integration with various transport layers. This library makes it easy to register methods and notifications, binding them to client logic efficiently.

## 🚀 Features

- **JSON-RPC 2.0**: Fully compliant server and client implementation.
- **Modern C++**: Written in C++17, ensuring a modern and efficient codebase.
- **Transport-Agnostic**: Abstract transport layer support.
- **Simple API**: Easy method and notification registration using lambda functions.

## 📦 Dependencies

- **CMake**: Version 3.19 or higher (for using CMake presets)
- **Ninja**: Recommended build system
- **C++ Compiler**: Must support C++17
- **Conan**: For dependency management

## 🛠 Build and Test

To build and test the project, follow these steps:

### Step 1: Install dependencies using Conan:

```bash
conan install . --output-folder=build --build=missing
```

### Step 2: Configure and build the project:

```bash
cmake --preset default
cmake --build --preset default
```

### Step 3: Run the tests:

```bash
ctest --preset default
```

These commands will install the necessary dependencies, configure the project, build it, and run the tests.

## 📋 Examples

### Client

```cpp
#include <iostream>
#include <memory>
#include "jsonrpc/jsonrpc.hpp"

using namespace jsonrpc;

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Client client(std::move(transport));

  // Perform addition
  Json response = client.SendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);

  // Perform division
  response = client.SendMethodCall("divide", {{"a", 10}, {"b", 0}}, 2);

  // Send stop notification
  client.SendNotification("stop", {});

  return 0;
}
```

### Server

```cpp
#include <memory>
#include "jsonrpc/jsonrpc.hpp"
#include "calculator.hpp"

using namespace jsonrpc;

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Server server(std::move(transport));
  Calculator calculator;

  server.RegisterMethodCall("add", [&calculator](const Json &params) {
    return calculator.Add(params);
  });

  server.RegisterMethodCall("divide", [&calculator](const Json &params) {
    return calculator.Divide(params);
  });

  server.RegisterNotification("stop", [&server](const Json &) {
    server.Stop();
  });

  server.Start();

  return 0;
}
```

These examples demonstrate a simple stdio JSON-RPC client and server. For examples using other transports, such as HTTP, please visit the [examples folder](https://github.com/hankhsu1996/json-rpc-2.0/tree/main/examples).

## 🤝 Contributing

We welcome contributions! If you have suggestions or find any issues, feel free to open an issue or pull request.

## 📄 License

This project is licensed under the MIT License. See the LICENSE file for more details.
