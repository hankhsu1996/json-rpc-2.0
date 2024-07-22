#pragma once

#include <optional>
#include <string>

#include "json_rpc/core/types.hpp"

namespace json_rpc {

class Request {
public:
  explicit Request(const std::string &method,
      const std::optional<Json> &params = std::nullopt,
      std::optional<Json> id = std::nullopt)
      : method_(method), params_(params), id_(id) {
  }

  // Parses a JSON object to create a Request object
  static Request FromJson(const Json &jsonObj);

  // Serializes the Request object to a JSON object
  Json ToJson() const;

  // Accessors
  inline std::string GetMethod() const {
    return method_;
  }

  inline std::optional<Json> GetParams() const {
    return params_;
  }

  inline std::optional<Json> GetId() const {
    return id_;
  }

private:
  std::string method_;
  std::optional<Json> params_;
  std::optional<Json> id_;
};

} // namespace json_rpc
