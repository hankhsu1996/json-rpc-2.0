#include <numeric>

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include "jsonrpc/server/dispatcher.hpp"

// Helper function to create a Dispatcher object
auto CreateDispatcher(
    bool enable_multithreading,
    size_t num_threads = std::thread::hardware_concurrency())
    -> std::unique_ptr<jsonrpc::server::Dispatcher> {
  return std::make_unique<jsonrpc::server::Dispatcher>(
      enable_multithreading, num_threads);
}

void RegisterSubtractHandler(jsonrpc::server::Dispatcher &dispatcher) {
  auto subtract =
      [](const std::optional<nlohmann::json> &params) -> nlohmann::json {
    int result = 0;
    if (params && params->is_array()) {
      result = params.value()[0].get<int>() - params.value()[1].get<int>();
    } else {
      result = params.value()["minuend"].get<int>() -
               params.value()["subtrahend"].get<int>();
    }
    return nlohmann::json{{"result", result}};
  };
  dispatcher.RegisterMethodCall("subtract", subtract);
}

void RegisterSumHandler(jsonrpc::server::Dispatcher &dispatcher) {
  auto sum = [](const std::optional<nlohmann::json> &params) -> nlohmann::json {
    int result = 0;
    if (params) {
      result = std::accumulate(
          params->begin(), params->end(), 0,
          [](int sum, const nlohmann::json &param) {
            return sum + param.get<int>();
          });
    }
    return nlohmann::json{{"result", result}};
  };
  dispatcher.RegisterMethodCall("sum", sum);
}

void RegisterGetDataHandler(jsonrpc::server::Dispatcher &dispatcher) {
  auto get_data = [](const std::optional<nlohmann::json> &) -> nlohmann::json {
    return nlohmann::json{{"result", nlohmann::json::array({"hello", 5})}};
  };
  dispatcher.RegisterMethodCall("get_data", get_data);
}

void RegisterNotificationHandlers(jsonrpc::server::Dispatcher &dispatcher) {
  auto notify_hello = [](const std::optional<nlohmann::json> &params) {
    if (params) {
      REQUIRE(params.value()[0] == 7);
    }
  };

  auto notify_sum = [](const std::optional<nlohmann::json> &params) {
    if (params) {
      REQUIRE(params.value().size() == 3);
    }
  };

  dispatcher.RegisterNotification("notify_hello", notify_hello);
  dispatcher.RegisterNotification("notify_sum", notify_sum);
}

void RegisterCommonHandlers(jsonrpc::server::Dispatcher &dispatcher) {
  RegisterSubtractHandler(dispatcher);
  RegisterSumHandler(dispatcher);
  RegisterGetDataHandler(dispatcher);
  RegisterNotificationHandlers(dispatcher);
}

TEST_CASE("RPC call with positional parameters", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("subtract 42, 23") {
    nlohmann::json request_json = {
        {"jsonrpc", "2.0"},
        {"method", "subtract"},
        {"params", {42, 23}},
        {"id", 1}};
    std::optional<std::string> response_str =
        dispatcher.DispatchRequest(request_json.dump());
    REQUIRE(response_str.has_value());
    nlohmann::json response_json = nlohmann::json::parse(response_str.value());
    REQUIRE(response_json["jsonrpc"] == "2.0");
    REQUIRE(response_json["result"] == 19);
    REQUIRE(response_json["id"] == 1);
  }

  SECTION("subtract 23, 42") {
    nlohmann::json request_json = {
        {"jsonrpc", "2.0"},
        {"method", "subtract"},
        {"params", {23, 42}},
        {"id", 2}};
    std::optional<std::string> response_str =
        dispatcher.DispatchRequest(request_json.dump());
    REQUIRE(response_str.has_value());
    nlohmann::json response_json = nlohmann::json::parse(response_str.value());
    REQUIRE(response_json["jsonrpc"] == "2.0");
    REQUIRE(response_json["result"] == -19);
    REQUIRE(response_json["id"] == 2);
  }
}

TEST_CASE("RPC call with named parameters", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("subtract named parameters 1") {
    nlohmann::json request_json = {
        {"jsonrpc", "2.0"},
        {"method", "subtract"},
        {"params", {{"subtrahend", 23}, {"minuend", 42}}},
        {"id", 3}};
    std::optional<std::string> response_str =
        dispatcher.DispatchRequest(request_json.dump());
    REQUIRE(response_str.has_value());
    nlohmann::json response_json = nlohmann::json::parse(response_str.value());
    REQUIRE(response_json["jsonrpc"] == "2.0");
    REQUIRE(response_json["result"] == 19);
    REQUIRE(response_json["id"] == 3);
  }

  SECTION("subtract named parameters 2") {
    nlohmann::json request_json = {
        {"jsonrpc", "2.0"},
        {"method", "subtract"},
        {"params", {{"minuend", 42}, {"subtrahend", 23}}},
        {"id", 4}};
    std::optional<std::string> response_str =
        dispatcher.DispatchRequest(request_json.dump());
    REQUIRE(response_str.has_value());
    nlohmann::json response_json = nlohmann::json::parse(response_str.value());
    REQUIRE(response_json["jsonrpc"] == "2.0");
    REQUIRE(response_json["result"] == 19);
    REQUIRE(response_json["id"] == 4);
  }
}

TEST_CASE("Notification", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  SECTION("update notification") {
    nlohmann::json request_json = {
        {"jsonrpc", "2.0"}, {"method", "update"}, {"params", {1, 2, 3, 4, 5}}};
    std::optional<std::string> response_str =
        dispatcher.DispatchRequest(request_json.dump());
    REQUIRE(!response_str.has_value());
  }

  SECTION("foobar notification") {
    nlohmann::json request_json = {{"jsonrpc", "2.0"}, {"method", "foobar"}};
    std::optional<std::string> response_str =
        dispatcher.DispatchRequest(request_json.dump());
    REQUIRE(!response_str.has_value());
  }
}

TEST_CASE("RPC call of non-existent method", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  nlohmann::json request_json = {
      {"jsonrpc", "2.0"}, {"method", "foobar"}, {"id", "1"}};
  std::optional<std::string> response_str =
      dispatcher.DispatchRequest(request_json.dump());
  REQUIRE(response_str.has_value());
  nlohmann::json response_json = nlohmann::json::parse(response_str.value());
  REQUIRE(response_json["jsonrpc"] == "2.0");
  REQUIRE(response_json["error"]["code"] == -32601);  // Method not found
  REQUIRE(response_json["error"]["message"] == "Method not found");
  REQUIRE(response_json["id"] == "1");
}

TEST_CASE("RPC call with invalid JSON", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;

  std::string invalid_request =
      R"({"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz])";
  std::optional<std::string> response_str =
      dispatcher.DispatchRequest(invalid_request);
  REQUIRE(response_str.has_value());
  nlohmann::json response_json = nlohmann::json::parse(response_str.value());
  REQUIRE(response_json["jsonrpc"] == "2.0");
  REQUIRE(response_json["error"]["code"] == -32700);  // Parse error
  REQUIRE(response_json["error"]["message"] == "Parse error");
  REQUIRE(response_json["id"] == nullptr);
}

TEST_CASE("RPC call with invalid ServerRequest object", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;

  nlohmann::json request_json = {
      {"jsonrpc", "2.0"}, {"method", 1}, {"params", "bar"}};
  std::optional<std::string> response_str =
      dispatcher.DispatchRequest(request_json.dump());
  REQUIRE(response_str.has_value());
  nlohmann::json response_json = nlohmann::json::parse(response_str.value());
  REQUIRE(response_json["jsonrpc"] == "2.0");
  REQUIRE(response_json["error"]["code"] == -32600);  // Invalid Request
  REQUIRE(response_json["error"]["message"] == "Invalid Request");
  REQUIRE(response_json["id"] == nullptr);
}

TEST_CASE("RPC call Batch, invalid JSON", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;

  std::string invalid_batch_request =
      R"([{"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id":
      "1"},{"jsonrpc": "2.0", "method"])";
  std::optional<std::string> response_str =
      dispatcher.DispatchRequest(invalid_batch_request);
  REQUIRE(response_str.has_value());
  nlohmann::json response_json = nlohmann::json::parse(response_str.value());
  REQUIRE(response_json["jsonrpc"] == "2.0");
  REQUIRE(response_json["error"]["code"] == -32700);  // Parse error
  REQUIRE(response_json["error"]["message"] == "Parse error");
  REQUIRE(response_json["id"] == nullptr);
}

TEST_CASE("RPC call with an empty Array", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;

  nlohmann::json request_json = nlohmann::json::array();
  std::optional<std::string> response_str =
      dispatcher.DispatchRequest(request_json.dump());
  REQUIRE(response_str.has_value());
  nlohmann::json response_json = nlohmann::json::parse(response_str.value());
  REQUIRE(response_json["jsonrpc"] == "2.0");
  REQUIRE(response_json["error"]["code"] == -32600);  // Invalid Request
  REQUIRE(response_json["error"]["message"] == "Invalid Request");
  REQUIRE(response_json["id"] == nullptr);
}

TEST_CASE("RPC call with an invalid Batch (but not empty)", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;

  nlohmann::json request_json = nlohmann::json::array({1});
  std::optional<std::string> response_str =
      dispatcher.DispatchRequest(request_json.dump());
  REQUIRE(response_str.has_value());
  nlohmann::json response_json = nlohmann::json::parse(response_str.value());
  REQUIRE(response_json.is_array());
  REQUIRE(response_json.size() == 1);
  REQUIRE(response_json[0]["jsonrpc"] == "2.0");
  REQUIRE(response_json[0]["error"]["code"] == -32600);  // Invalid Request
  REQUIRE(response_json[0]["error"]["message"] == "Invalid Request");
  REQUIRE(response_json[0]["id"] == nullptr);
}

TEST_CASE("RPC call with invalid Batch", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;

  nlohmann::json request_json = nlohmann::json::array({1, 2, 3});
  std::optional<std::string> response_str =
      dispatcher.DispatchRequest(request_json.dump());
  REQUIRE(response_str.has_value());
  nlohmann::json response_json = nlohmann::json::parse(response_str.value());
  REQUIRE(response_json.is_array());
  REQUIRE(response_json.size() == 3);
  for (const auto &response : response_json) {
    REQUIRE(response["jsonrpc"] == "2.0");
    REQUIRE(response["error"]["code"] == -32600);  // Invalid Request
    REQUIRE(response["error"]["message"] == "Invalid Request");
    REQUIRE(response["id"] == nullptr);
  }
}

TEST_CASE("RPC call Batch", "[Dispatcher]") {
  auto dispatcher = CreateDispatcher(true);
  RegisterCommonHandlers(*dispatcher);

  // clang-format off
  nlohmann::json request_json = nlohmann::json::array({
    {{"jsonrpc", "2.0"}, {"method", "sum"}, {"params", {1, 2, 4}}, {"id", "1"}},
    {{"jsonrpc", "2.0"}, {"method", "notify_hello"}, {"params", {7}}},
    {{"jsonrpc", "2.0"}, {"method", "subtract"}, {"params", {42, 23}}, {"id", "2"}},
    {{"foo", "boo"}},
    {{"jsonrpc", "2.0"}, {"method", "foo.get"}, {"params", {{"name", "myself"}}}, {"id", "5"}},
    {{"jsonrpc", "2.0"}, {"method", "get_data"}, {"id", "9"}}
  });
  // clang-format on

  std::optional<std::string> response_str =
      dispatcher->DispatchRequest(request_json.dump());
  REQUIRE(response_str.has_value());
  nlohmann::json response_json = nlohmann::json::parse(response_str.value());

  REQUIRE(response_json.is_array());
  REQUIRE(response_json.size() == 5);  // 6 requests but one is a notification

  REQUIRE(response_json[0]["jsonrpc"] == "2.0");
  REQUIRE(response_json[0]["result"] == 7);
  REQUIRE(response_json[0]["id"] == "1");

  REQUIRE(response_json[1]["jsonrpc"] == "2.0");
  REQUIRE(response_json[1]["result"] == 19);
  REQUIRE(response_json[1]["id"] == "2");

  REQUIRE(response_json[2]["jsonrpc"] == "2.0");
  REQUIRE(response_json[2]["error"]["code"] == -32600);  // Invalid Request
  REQUIRE(response_json[2]["error"]["message"] == "Invalid Request");
  REQUIRE(response_json[2]["id"] == nullptr);

  REQUIRE(response_json[3]["jsonrpc"] == "2.0");
  REQUIRE(response_json[3]["error"]["code"] == -32601);  // Method not found
  REQUIRE(response_json[3]["error"]["message"] == "Method not found");
  REQUIRE(response_json[3]["id"] == "5");

  REQUIRE(response_json[4]["jsonrpc"] == "2.0");
  REQUIRE(response_json[4]["result"] == nlohmann::json::array({"hello", 5}));
  REQUIRE(response_json[4]["id"] == "9");

  dispatcher = CreateDispatcher(false);
  RegisterCommonHandlers(*dispatcher);
  response_str = dispatcher->DispatchRequest(request_json.dump());
  REQUIRE(response_str.has_value());
  response_json = nlohmann::json::parse(response_str.value());
  REQUIRE(response_json.is_array());
  REQUIRE(response_json.size() == 5);  // 6 requests but one is a notification

  REQUIRE(response_json[0]["jsonrpc"] == "2.0");
  REQUIRE(response_json[0]["result"] == 7);
  REQUIRE(response_json[0]["id"] == "1");

  REQUIRE(response_json[1]["jsonrpc"] == "2.0");
  REQUIRE(response_json[1]["result"] == 19);
  REQUIRE(response_json[1]["id"] == "2");

  REQUIRE(response_json[2]["jsonrpc"] == "2.0");
  REQUIRE(response_json[2]["error"]["code"] == -32600);  // Invalid Request
  REQUIRE(response_json[2]["error"]["message"] == "Invalid Request");
  REQUIRE(response_json[2]["id"] == nullptr);

  REQUIRE(response_json[3]["jsonrpc"] == "2.0");
  REQUIRE(response_json[3]["error"]["code"] == -32601);  // Method not found
  REQUIRE(response_json[3]["error"]["message"] == "Method not found");
  REQUIRE(response_json[3]["id"] == "5");

  REQUIRE(response_json[4]["jsonrpc"] == "2.0");
  REQUIRE(response_json[4]["result"] == nlohmann::json::array({"hello", 5}));
  REQUIRE(response_json[4]["id"] == "9");
}

TEST_CASE("RPC call Batch (all notifications)", "[Dispatcher]") {
  jsonrpc::server::Dispatcher dispatcher;
  RegisterCommonHandlers(dispatcher);

  nlohmann::json request_json = nlohmann::json::array(
      {{{"jsonrpc", "2.0"}, {"method", "notify_sum"}, {"params", {1, 2, 4}}},
       {{"jsonrpc", "2.0"}, {"method", "notify_hello"}, {"params", {7}}}});
  std::optional<std::string> response_str =
      dispatcher.DispatchRequest(request_json.dump());
  REQUIRE(!response_str.has_value());
}
