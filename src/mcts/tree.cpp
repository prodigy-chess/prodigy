module;

#include <cstdint>
#include <span>

module prodigy.mcts;

import prodigy.core;
import prodigy.move_generator;

import :expand;

namespace prodigy::mcts {
Edge::Edge(const QuietMove& move) noexcept : quiet_move_(move), move_type_(MoveType::QUIET_MOVE) {}

Edge::Edge(const QuietMove& move, EnableEnPassant) noexcept
    : quiet_move_(move), move_type_(MoveType::ENABLE_EN_PASSANT) {}

Edge::Edge(const QuietMove& move, const CastlingRights child_castling_rights) noexcept
    : quiet_move_(move),
      move_type_(MoveType::QUIET_MOVE_NEW_CASTLING_RIGHTS),
      child_castling_rights_(child_castling_rights) {}

Edge::Edge(const Capture& move) noexcept : capture_(move), move_type_(MoveType::CAPTURE) {}

Edge::Edge(const Capture& move, const CastlingRights child_castling_rights) noexcept
    : capture_(move),
      move_type_(MoveType::CAPTURE_NEW_CASTLING_RIGHTS),
      child_castling_rights_(child_castling_rights) {}

Edge::Edge(const KingsideCastle&) noexcept : move_type_(MoveType::KINGSIDE_CASTLE) {}

Edge::Edge(const QueensideCastle&) noexcept : move_type_(MoveType::QUEENSIDE_CASTLE) {}

Edge::Edge(const QuietPromotion& move) noexcept : quiet_promotion_(move), move_type_(MoveType::QUIET_PROMOTION) {}

Edge::Edge(const CapturePromotion& move) noexcept : capture_promotion_(move), move_type_(MoveType::CAPTURE_PROMOTION) {}

Edge::Edge(const CapturePromotion& move, const CastlingRights child_castling_rights) noexcept
    : capture_promotion_(move),
      move_type_(MoveType::CAPTURE_PROMOTION_NEW_CASTLING_RIGHTS),
      child_castling_rights_(child_castling_rights) {}

Edge::Edge(const EnPassant& move) noexcept : en_passant_(move), move_type_(MoveType::EN_PASSANT) {}

Node::Node(const EdgeCount edge_count, const bool is_check) noexcept : edge_count_(edge_count), is_check_(is_check) {}

std::span<Edge> Node::edges() noexcept { return {reinterpret_cast<Edge*>(this + 1), edge_count_}; }

bool Node::is_check() const noexcept { return is_check_; }

Tree::Tree(const Position& position)
    : position_(position),
      root_(move_generator::dispatch(
          position_, [&]<auto context>(const auto& node) -> decltype(auto) { return expand<context>(node, arena_); })) {
}

const Position& Tree::position() const noexcept { return position_; }

Node& Tree::root() noexcept { return root_; }
}  // namespace prodigy::mcts
