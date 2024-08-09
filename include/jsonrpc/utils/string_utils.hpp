#pragma once

#include <cctype>
#include <ranges>
#include <string>
#include <string_view>

namespace jsonrpc::utils {

auto Trim(std::string_view in) -> std::string {
  auto view = in | std::views::drop_while(::isspace) | std::views::reverse |
              std::views::drop_while(::isspace) | std::views::reverse;
  return {view.begin(), view.end()};
}

}  // namespace jsonrpc::utils
