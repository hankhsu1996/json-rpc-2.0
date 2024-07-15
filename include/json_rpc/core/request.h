#pragma once
#ifndef REQUEST_H
#define REQUEST_H

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace json_rpc {

class Request {
public:
  Request(const std::string &method, const nlohmann::json &params,
      std::optional<int> id);

  // Parses a JSON object to create a Request object
  static Request from_json(const nlohmann::json &json_obj);

  // Serializes the Request object to a JSON object
  nlohmann::json to_json() const;

  // Accessors
  std::string get_method() const;
  nlohmann::json get_params() const;
  std::optional<int> get_id() const;

private:
  std::string _method;
  nlohmann::json _params;
  std::optional<int> _id;
};

} // namespace json_rpc

#endif // REQUEST_H
