#pragma once

#include <optional>
#include <string>

#include "json_rpc/core/types.hpp"

namespace json_rpc {

class Request {
public:
  Request(const std::string &method, const std::optional<Json> &params,
      std::optional<Json> id);

  // Parses a JSON object to create a Request object
  static Request FromJson(const Json &jsonObj);

  // Serializes the Request object to a JSON object
  Json ToJson() const;

  // Accessors
  std::string GetMethod() const;
  std::optional<Json> GetParams() const;
  std::optional<Json> GetId() const;

private:
  std::string method_;
  std::optional<Json> params_;
  std::optional<Json> id_;
};

} // namespace json_rpc
