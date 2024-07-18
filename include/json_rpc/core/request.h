#pragma once

#include <optional>
#include <string>

#include "json_rpc/core/types.h"

namespace json_rpc {

class Request {
public:
  Request(const std::string &method, const Json &params, std::optional<int> id);

  // Parses a JSON object to create a Request object
  static Request FromJson(const Json &jsonObj);

  // Serializes the Request object to a JSON object
  Json ToJson() const;

  // Accessors
  std::string GetMethod() const;
  Json GetParams() const;
  std::optional<int> GetId() const;

private:
  std::string method_;
  Json params_;
  std::optional<int> id_;
};

} // namespace json_rpc
