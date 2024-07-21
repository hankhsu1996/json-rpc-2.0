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
    Json response = {{"result", a + b}};
    spdlog::info("Sending add response: {}", response.dump());
    return response;
  }

  Json Divide(const Json &params) {
    spdlog::info("Received divide request with params: {}", params.dump());
    double a = params["a"];
    double b = params["b"];
    if (b == 0) {
      Json errorResponse = {
          {"error", {{"code", -32602}, {"message", "Division by zero"}}}};
      spdlog::error("Sending divide error response: {}", errorResponse.dump());
      return errorResponse;
    }
    Json response = {{"result", a / b}};
    spdlog::info("Sending divide response: {}", response.dump());
    return response;
  }
};
