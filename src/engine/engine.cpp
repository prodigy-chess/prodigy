module;

#include <algorithm>
#include <asio/steady_timer.hpp>
#include <cassert>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

module prodigy.engine;

import prodigy.core;
import prodigy.move_generator;
import prodigy.uci;

namespace prodigy {
Engine::Engine(asio::io_context& io_context, std::unique_ptr<Strategy> strategy,
               const std::chrono::steady_clock::duration poll_interval)
    : uci::Engine(io_context), timer_(io_context), strategy_(std::move(strategy)), poll_interval_(poll_interval) {
  assert(strategy_ != nullptr);
}

void Engine::set_position(const Position& position) { position_ = position; }

namespace {
class Visitor : public move_generator::Visitor<Visitor> {
 public:
  explicit Visitor(Position& position, const uci::Move move) noexcept : position_(position), move_(move) {}

  template <move_generator::Node::Context child_context>
  void visit_pawn_move(const auto& move) const noexcept {
    visit<PieceType::PAWN, child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_knight_move(const auto& move) const noexcept {
    visit<PieceType::KNIGHT, child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_bishop_move(const auto& move) const noexcept {
    visit<PieceType::BISHOP, child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_rook_move(const auto& move) const noexcept {
    visit<PieceType::ROOK, child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_queen_move(const auto& move) const noexcept {
    visit<PieceType::QUEEN, child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_king_move(const auto& move) const noexcept {
    visit<PieceType::KING, child_context>(move);
  }

  void is_check() const noexcept {}

 private:
  template <PieceType piece_type, move_generator::Node::Context child_context>
  void visit(const auto& move) const noexcept {
    if (uci::to_move(move) != move_) {
      return;
    }
    auto& [board, side_to_move, castling_rights, en_passant_target, halfmove_clock, fullmove_number] = position_;
    board.apply<!child_context.side_to_move>(move);
    side_to_move = child_context.side_to_move;
    castling_rights = child_context.castling_rights;
    if constexpr (child_context.can_en_passant) {
      en_passant_target = square_of(
          unsafe_shift(move.target, !child_context.side_to_move == Color::WHITE ? Direction::SOUTH : Direction::NORTH));
    } else {
      en_passant_target.reset();
    }
    if constexpr (std::same_as<std::remove_cvref_t<decltype(move)>, Capture> || piece_type == PieceType::PAWN) {
      halfmove_clock = Ply{0};
    } else {
      halfmove_clock = static_cast<Ply>(std::to_underlying(halfmove_clock) + 1);
    }
    if constexpr (child_context.side_to_move == Color::WHITE) {
      ++fullmove_number;
    }
  }

  Position& position_;
  const uci::Move move_;
};
}  // namespace

void Engine::apply(const uci::Move move) {
  move_generator::dispatch(position_, [&]<auto context>(const auto& node) {
    move_generator::walk<context>(node, Visitor(position_, move));
  });
}

void Engine::go(const uci::Go& params) {
  strategy_->start(position_, params.nodes);
  if (params.infinite) {
    search_expiry_.reset();
  } else {
    search_expiry_.emplace(
        std::chrono::steady_clock::now() +
        params.move_time
            .or_else([&] {
              return params.time_remaining[position_.side_to_move].transform([&](const auto time_remaining) {
                return time_remaining / static_cast<std::ptrdiff_t>(params.moves_to_go.value_or(40UZ));
              });
            })
            .value());
  }
  async_poll();
}

void Engine::stop() { strategy_->stop(); }

void Engine::async_poll() {
  timer_.expires_after(poll_interval_);
  if (search_expiry_.has_value()) {
    timer_.expires_at(std::min(*search_expiry_, timer_.expiry()));
  }
  timer_.async_wait([&](const asio::error_code& error) {
    if (error) {
      return;
    }
    if ((search_expiry_.has_value() && *search_expiry_ < std::chrono::steady_clock::now()) || strategy_->poll()) {
      stop();
      if (const auto move = strategy_->join(); move.has_value()) {
        std::cout << "bestmove " << *move << std::endl;
      } else {
        std::cout << "bestmove 0000" << std::endl;
      }
      return;
    }
    async_poll();
  });
}
}  // namespace prodigy
