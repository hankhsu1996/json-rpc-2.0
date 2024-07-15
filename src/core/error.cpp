#include "json_rpc/core/error.h"

namespace json_rpc {

const int Error::PARSE_ERROR = -32700;
const int Error::INVALID_REQUEST = -32600;
const int Error::METHOD_NOT_FOUND = -32601;
const int Error::INVALID_PARAMS = -32602;
const int Error::INTERNAL_ERROR = -32603;

Error::Error(int code, const std::string &message)
    : _code(code), _message(message) {
}

Error Error::from_json(const nlohmann::json &json_obj) {
  return Error(json_obj["code"], json_obj["message"]);
}

nlohmann::json Error::to_json() const {
  return {{"code", _code}, {"message", _message}};
}

int Error::get_code() const {
  return _code;
}

std::string Error::get_message() const {
  return _message;
}

} // namespace json_rpc
