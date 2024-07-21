# JSON-RPC 2.0 C++ Implementation

This project is a lightweight, modern C++ library for implementing JSON-RPC 2.0 servers and clients. It is designed to be flexible, allowing integration with various transport layers. This library makes it easy to register methods and notifications, binding them to client logic efficiently.

## Features

- JSON-RPC 2.0 server and client.
- Implemented in modern C++ (C++17).
- Abstract transport layer support, making it transport-agnostic.
- Simple registration of methods and notifications using lambda functions.

## Examples

### Client

```cpp
#include <iostream>
#include <memory>
#include "json_rpc/json_rpc.h"

using namespace json_rpc;

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Client client(std::move(transport));

  // Perform addition
  Json response = client.SendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);

  // Send stop notification
  client.SendNotification("stop", {});

  return 0;
}
```

### Server

```cpp
#include <memory>
#include "json_rpc/json_rpc.h"
#include "calculator.h"

using namespace json_rpc;

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Server server(std::move(transport));
  Calculator calculator;

  server.RegisterMethodCall("add",
      [&calculator](const Json &params) { return calculator.Add(params); });

  server.RegisterNotification("stop", [&server](const Json &) {
    server.Stop();
  });

  server.Start();

  return 0;
}
```

These examples demonstrate a simple stdio JSON-RPC client and server. For examples using other transports, such as HTTP, please visit the [examples folder](https://github.com/hankhsu1996/json-rpc-2.0/tree/main/examples).

## Contributing

We welcome contributions! If you have suggestions or find any issues, feel free to open an issue or pull request.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.
