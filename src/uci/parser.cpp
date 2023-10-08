module;

#include <cassert>
#include <memory>
#include <string_view>
#include <utility>

module prodigy.uci;

namespace prodigy::uci {
Parser::Parser(std::unique_ptr<Engine> engine) noexcept : engine_(std::move(engine)) { assert(engine_ != nullptr); }

bool Parser::handle(std::string_view command) const noexcept {
  while (!command.empty()) {
    static constexpr auto WHITESPACE = " \f\n\r\t\v";
    const auto begin = command.find_first_not_of(WHITESPACE);
    if (begin == std::string_view::npos) {
      break;
    }
    const auto end = command.find_first_of(WHITESPACE, begin);
    const auto token = command.substr(begin, end - begin);
    if (token == "stop") {
      engine_->stop();
      break;
    }
    if (token == "quit") {
      return false;
    }
    command.remove_prefix(end);
  }
  return true;
}
}  // namespace prodigy::uci
