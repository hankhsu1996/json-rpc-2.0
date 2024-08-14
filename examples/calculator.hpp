#pragma once

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

const int kDivideByZeroErrorCode = -32602;

class Calculator {
 public:
  static auto Add(const nlohmann::json &params) -> nlohmann::json {
    spdlog::debug("Received add request with params: {}", params.dump());

    double a_double = params["a"];
    double b_double = params["b"];

    return {{"result", a_double + b_double}};
  }

  static auto Divide(const nlohmann::json &params) -> nlohmann::json {
    spdlog::debug("Received divide request with params: {}", params.dump());

    double a_double = params["a"];
    double b_double = params["b"];

    if (b_double == 0) {
      return {
          {"error",
           {{"code", kDivideByZeroErrorCode},
            {"message", "Division by zero"}}}};
    }

    return {{"result", a_double / b_double}};
  }
};
