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
#include <nlohmann/json.hpp>
#include "json_rpc/client/client.h"
#include "json_rpc/transports/stdio_transport.h"

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Client client(std::move(transport));

  // Perform addition
  nlohmann::json response =
      client.SendMethodCall("add", {{"a", 10}, {"b", 5}}, 1);
  std::cout << "Addition result: " << response.dump() << std::endl;

  // Log a notification
  client.SendNotification("log", {{"message", "Performed addition"}});

  return 0;
}
```

### Server

```cpp
#include "json_rpc/server/server.h"
#include "json_rpc/transports/stdio_transport.h"
#include "calculator.h"

int main() {
  auto transport = std::make_unique<StdioTransport>();
  Server server(std::move(transport));

  Calculator calculator;
  server.RegisterMethodCall("add", [&calculator](const nlohmann::json &params) {
    return calculator.Add(params);
  });
  server.RegisterNotification("log",
      [&calculator](const nlohmann::json &params) { calculator.Log(params); });

  server.Start();

  return 0;
}
```

## Contributing

We welcome contributions! If you have suggestions or find any issues, feel free to open an issue or pull request.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.
