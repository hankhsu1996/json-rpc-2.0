#pragma once

#include <cctype>
#include <ranges>
#include <string>
#include <string_view>

namespace jsonrpc {
namespace utils {

std::string trim(std::string_view in) {
  auto view = in | std::views::drop_while(::isspace) | std::views::reverse |
              std::views::drop_while(::isspace) | std::views::reverse;
  return {view.begin(), view.end()};
}

} // namespace utils
} // namespace jsonrpc
