#pragma once
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <iostream>

#include <nlohmann/json.hpp>

class Calculator {
public:
  nlohmann::json add(const nlohmann::json &params) {
    std::cerr << "Server: Received add request with params: " << params.dump()
              << std::endl;
    if (params.contains("a") && params.contains("b")) {
      double a = params["a"];
      double b = params["b"];
      nlohmann::json response = {{"result", a + b}};
      std::cerr << "Server: Sending add response: " << response.dump()
                << std::endl;
      return response;
    }
    nlohmann::json error_response = {
        {"error", {{"code", -32602}, {"message", "Invalid parameters"}}}};
    std::cerr << "Server: Sending add error response: " << error_response.dump()
              << std::endl;
    return error_response;
  }

  nlohmann::json divide(const nlohmann::json &params) {
    std::cerr << "Server: Received divide request with params: "
              << params.dump() << std::endl;
    if (params.contains("a") && params.contains("b")) {
      double a = params["a"];
      double b = params["b"];
      if (b == 0) {
        nlohmann::json error_response = {
            {"error", {{"code", -32602}, {"message", "Division by zero"}}}};
        std::cerr << "Server: Sending divide error response: "
                  << error_response.dump() << std::endl;
        return error_response;
      }
      nlohmann::json response = {{"result", a / b}};
      std::cerr << "Server: Sending divide response: " << response.dump()
                << std::endl;
      return response;
    }
    nlohmann::json error_response = {
        {"error", {{"code", -32602}, {"message", "Invalid parameters"}}}};
    std::cerr << "Server: Sending divide error response: "
              << error_response.dump() << std::endl;
    return error_response;
  }

  void log(const nlohmann::json &params) {
    std::cerr << "Server: Received log notification with params: "
              << params.dump() << std::endl;
    // Logging is a notification, so it doesn't return a result or error
    return;
  }
};

#endif // CALCULATOR_H
