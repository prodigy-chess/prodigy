module;

#include <asio/steady_timer.hpp>
#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>

module prodigy.engine;

import prodigy.core;
import prodigy.move_generator;
import prodigy.uci;

namespace prodigy {
Engine::Engine(asio::io_context& io_context) : uci::Engine(io_context), timer_(io_context) {}

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

void Engine::set_position(const Position& position) { position_ = position; }

void Engine::apply(const uci::Move move) {
  move_generator::dispatch(position_, [&]<auto context>(const auto& node) {
    move_generator::walk<context>(node, Visitor(position_, move));
  });
}

void Engine::go(const uci::Go&) {}

void Engine::stop() {}
}  // namespace prodigy
