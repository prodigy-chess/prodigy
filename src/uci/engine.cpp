module;

#include <algorithm>
#include <iostream>
#include <string_view>

#include "metadata.h"

module prodigy.uci;

import prodigy.core;

namespace prodigy::uci {
bool Engine::handle(std::string_view command) {
  static constexpr auto WHITESPACE = " \f\r\t\v";
  const auto pop_token = [&] -> std::string_view {
    const auto begin = command.find_first_not_of(WHITESPACE);
    if (begin == std::string_view::npos) {
      return {};
    }
    const auto end = std::min(command.find_first_of(WHITESPACE, begin), command.size());
    const auto token = command.substr(begin, end - begin);
    command.remove_prefix(end);
    return token;
  };
  for (auto token = pop_token(); !token.empty(); token = pop_token()) {
    if (token == "uci") {
      std::cout << "id name " << PROJECT_NAME << ' ' << PROJECT_VERSION << '@' << GIT_HASH << '\n';
      std::cout << "id author " << PROJECT_AUTHOR << '\n';
      std::cout << "uciok" << std::endl;
      break;
    }
    if (token == "debug") {
      break;
    }
    if (token == "isready") {
      std::cout << "readyok" << std::endl;
      break;
    }
    if (token == "setoption") {
      break;
    }
    if (token == "register") {
      break;
    }
    if (token == "ucinewgame") {
      break;
    }
    if (token == "position") {
      if (pop_token() == "startpos") {
        set_position(STARTING_POSITION);
      } else {
        const auto begin = command.find_first_not_of(WHITESPACE);
        const auto end = command.find_last_not_of(WHITESPACE, command.find("moves", begin) - 1) + 1;
        set_position(parse_fen(command.substr(begin, end - begin)).value());
        command.remove_prefix(end);
      }
      pop_token();
      for (auto move = pop_token(); !move.empty(); move = pop_token()) {
        apply(parse_move(move).value());
      }
      break;
    }
    if (token == "go") {
      // TODO
      break;
    }
    if (token == "stop") {
      stop();
      break;
    }
    if (token == "ponderhit") {
      break;
    }
    if (token == "quit") {
      return false;
    }
  }
  return true;
}
}  // namespace prodigy::uci
