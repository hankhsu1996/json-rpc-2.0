#pragma once

#include <iostream>

#include <nlohmann/json.hpp>

using Json = nlohmann::json;

class Calculator {

public:
  Json Add(const Json &params) {
    std::cerr << "Server: Received add request with params: " << params.dump()
              << std::endl;
    if (params.contains("a") && params.contains("b")) {
      double a = params["a"];
      double b = params["b"];
      Json response = {{"result", a + b}};
      std::cerr << "Server: Sending add response: " << response.dump()
                << std::endl;
      return response;
    }
    Json errorResponse = {
        {"error", {{"code", -32602}, {"message", "Invalid parameters"}}}};
    std::cerr << "Server: Sending add error response: " << errorResponse.dump()
              << std::endl;
    return errorResponse;
  }

  Json Divide(const Json &params) {
    std::cerr << "Server: Received divide request with params: "
              << params.dump() << std::endl;
    if (params.contains("a") && params.contains("b")) {
      double a = params["a"];
      double b = params["b"];
      if (b == 0) {
        Json errorResponse = {
            {"error", {{"code", -32602}, {"message", "Division by zero"}}}};
        std::cerr << "Server: Sending divide error response: "
                  << errorResponse.dump() << std::endl;
        return errorResponse;
      }
      Json response = {{"result", a / b}};
      std::cerr << "Server: Sending divide response: " << response.dump()
                << std::endl;
      return response;
    }
    Json errorResponse = {
        {"error", {{"code", -32602}, {"message", "Invalid parameters"}}}};
    std::cerr << "Server: Sending divide error response: "
              << errorResponse.dump() << std::endl;
    return errorResponse;
  }

  void Log(const Json &params) {
    std::cerr << "Server: Received log notification with params: "
              << params.dump() << std::endl;
    // Logging is a notification, so it doesn't return a result or error
    return;
  }
};
