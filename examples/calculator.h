#pragma once
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <iostream>

#include <nlohmann/json.hpp>

#include "json_rpc/core/error.h"
#include "json_rpc/core/request.h"
#include "json_rpc/core/response.h"

// JSON-RPC Specification for Calculator Application
//
// 1. Add
//    Method: "add"
//    Params: { "a": number, "b": number }
//    Result: { "result": number }
//
// 2. Divide
//    Method: "divide"
//    Params: { "a": number, "b": number }
//    Result: { "result": number }
//    Error: { "code": -32602, "message": "Division by zero" if b is 0 }
//
// 3. Log
//    Method: "log"
//    Params: { "message": string }

class Calculator {
public:
  json_rpc::Response add(const json_rpc::Request &request) {
    nlohmann::json params = request.get_params();
    std::cerr << "Server: Received add request with params: " << params.dump()
              << std::endl;
    if (params.contains("a") && params.contains("b")) {
      double a = params["a"];
      double b = params["b"];
      auto response = json_rpc::Response({{"result", a + b}}, request.get_id());
      std::cerr << "Server: Sending add response: " << response.to_json().dump()
                << std::endl;
      return response;
    }
    auto error_response = json_rpc::Response(
        json_rpc::Error(json_rpc::Error::INVALID_PARAMS, "Invalid parameters"),
        request.get_id());
    std::cerr << "Server: Sending add error response: "
              << error_response.to_json().dump() << std::endl;
    return error_response;
  }

  json_rpc::Response divide(const json_rpc::Request &request) {
    nlohmann::json params = request.get_params();
    std::cerr << "Server: Received divide request with params: "
              << params.dump() << std::endl;
    if (params.contains("a") && params.contains("b")) {
      double a = params["a"];
      double b = params["b"];
      if (b == 0) {
        auto error_response =
            json_rpc::Response(json_rpc::Error(json_rpc::Error::INVALID_PARAMS,
                                   "Division by zero"),
                request.get_id());
        std::cerr << "Server: Sending divide error response: "
                  << error_response.to_json().dump() << std::endl;
        return error_response;
      }
      auto response = json_rpc::Response({{"result", a / b}}, request.get_id());
      std::cerr << "Server: Sending divide response: "
                << response.to_json().dump() << std::endl;
      return response;
    }
    auto error_response = json_rpc::Response(
        json_rpc::Error(json_rpc::Error::INVALID_PARAMS, "Invalid parameters"),
        request.get_id());
    std::cerr << "Server: Sending divide error response: "
              << error_response.to_json().dump() << std::endl;
    return error_response;
  }

  void log(const json_rpc::Request &request) {
    nlohmann::json params = request.get_params();
    std::cerr << "Server: Received log notification with params: "
              << params.dump() << std::endl;
  }
};

#endif // CALCULATOR_H
