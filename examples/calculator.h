#pragma once

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using Json = nlohmann::json;

class Calculator {
public:
  Json Add(const Json &params) {
    spdlog::info("Received add request with params: {}", params.dump());
    double a = params["a"];
    double b = params["b"];
    return {{"result", a + b}};
  }

  Json Divide(const Json &params) {
    spdlog::info("Received divide request with params: {}", params.dump());
    double a = params["a"];
    double b = params["b"];
    if (b == 0) {
      return {{"error", {{"code", -32602}, {"message", "Division by zero"}}}};
    }
    return {{"result", a / b}};
  }
};
