#pragma once
#ifndef ERROR_H
#define ERROR_H

#include <string>

#include <nlohmann/json.hpp>

namespace json_rpc {

class Error {
public:
  Error(int code, const std::string &message);

  // Parses a JSON object to create an Error object
  static Error from_json(const nlohmann::json &json_obj);

  // Serializes the Error object to a JSON object
  nlohmann::json to_json() const;

  // Accessors
  int get_code() const;
  std::string get_message() const;

  // Standard JSON-RPC error codes
  static const int PARSE_ERROR;
  static const int INVALID_REQUEST;
  static const int METHOD_NOT_FOUND;
  static const int INVALID_PARAMS;
  static const int INTERNAL_ERROR;

private:
  int _code;
  std::string _message;
};

} // namespace json_rpc

#endif // ERROR_H
