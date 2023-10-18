module;

#include <algorithm>
#include <asio/io_context.hpp>
#include <charconv>
#include <concepts>
#include <expected>
#include <iostream>
#include <string_view>
#include <utility>

#include "metadata.h"

module prodigy.uci;

import prodigy.core;

namespace prodigy::uci {
Engine::Engine(asio::io_context& io_context) noexcept : io_context_(io_context) {}

namespace {
template <typename T>
std::expected<T, std::string_view> parse(const std::string_view token) {
  auto value = [] {
    if constexpr (std::integral<T>) {
      return T();
    } else {
      return typename T::rep();
    }
  }();
  if (std::from_chars(token.begin(), token.end(), value) != std::from_chars_result{token.end(), std::errc()}) {
    return std::unexpected(token);
  }
  return T(value);
}
}  // namespace

void Engine::handle(std::string_view command) {
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
      Go params;
      bool parsing_search_moves = false;
      for (auto token = pop_token(); !token.empty(); token = pop_token()) {
        if (token == "searchmoves") {
          parsing_search_moves = true;
          continue;
        }
        if (parsing_search_moves) {
          if (const auto move = parse_move(token); move.has_value()) {
            params.search_moves.push_back(*move);
            continue;
          }
          parsing_search_moves = false;
        }
#define _(TOKEN, PARAM, PARSE) \
  if (token == TOKEN) {        \
    params.PARAM = PARSE;      \
    continue;                  \
  }
        _("ponder", ponder, true)
        _("wtime", time_remaining[Color::WHITE],
          parse<typename decltype(params.time_remaining)::value_type::value_type>(pop_token()).value())
        _("btime", time_remaining[Color::BLACK],
          parse<typename decltype(params.time_remaining)::value_type::value_type>(pop_token()).value())
        _("winc", increment[Color::WHITE], parse<decltype(params.increment)::value_type>(pop_token()).value())
        _("binc", increment[Color::BLACK], parse<decltype(params.increment)::value_type>(pop_token()).value())
        _("movestogo", moves_to_go, parse<decltype(params.moves_to_go)::value_type>(pop_token()).value())
        _("depth", depth, Ply{parse<std::underlying_type_t<decltype(params.depth)::value_type>>(pop_token()).value()})
        _("nodes", nodes, parse<decltype(params.nodes)::value_type>(pop_token()).value())
        _("mate", mate, parse<decltype(params.mate)::value_type>(pop_token()).value())
        _("movetime", move_time, parse<decltype(params.move_time)::value_type>(pop_token()).value())
        _("infinite", infinite, true)
#undef _
      }
      go(params);
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
      io_context_.stop();
      break;
    }
  }
}
}  // namespace prodigy::uci
