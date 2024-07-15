#pragma once
#ifndef RESPONSE_H
#define RESPONSE_H

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include "error.h"

namespace json_rpc {

class Response {
public:
  Response(const nlohmann::json &result, std::optional<int> id);
  Response(const Error &error, std::optional<int> id);

  // Parses a JSON object to create a Response object
  static Response from_json(const nlohmann::json &json_obj);

  // Serializes the Response object to a JSON object
  nlohmann::json to_json() const;

  // Accessors
  nlohmann::json get_result() const;
  Error get_error() const;
  std::optional<int> get_id() const;
  bool is_success() const;

private:
  nlohmann::json _result;
  Error _error;
  std::optional<int> _id;
  bool _success;
};

} // namespace json_rpc

#endif // RESPONSE_H
