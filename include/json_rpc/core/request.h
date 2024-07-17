#pragma once
#ifndef JSON_RPC_REQUEST_H
#define JSON_RPC_REQUEST_H

#include <optional>
#include <string>

#include "json_rpc/core/types.h"

namespace json_rpc {

class Request {
public:
  Request(const std::string &method, const json &params, std::optional<int> id);

  // Parses a JSON object to create a Request object
  static Request from_json(const json &json_obj);

  // Serializes the Request object to a JSON object
  json to_json() const;

  // Accessors
  std::string get_method() const;
  json get_params() const;
  std::optional<int> get_id() const;

private:
  std::string _method;
  json _params;
  std::optional<int> _id;
};

} // namespace json_rpc

#endif // JSON_RPC_REQUEST_H
