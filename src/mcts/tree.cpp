module;

#include <cstdint>
#include <span>

module prodigy.mcts;

import prodigy.core;
import prodigy.move_generator;

import :expand;

namespace prodigy::mcts {
Edge::Edge(const QuietMove& move, const CastlingRights child_castling_rights, const bool child_can_en_passant) noexcept
    : quiet_move_(move),
      move_type_(MoveType::QUIET_MOVE),
      child_castling_rights_(child_castling_rights),
      child_can_en_passant_(child_can_en_passant) {}

Edge::Edge(const Capture& move, const CastlingRights child_castling_rights, const bool child_can_en_passant) noexcept
    : capture_(move),
      move_type_(MoveType::CAPTURE),
      child_castling_rights_(child_castling_rights),
      child_can_en_passant_(child_can_en_passant) {}

Edge::Edge(const Castle& move, const CastlingRights child_castling_rights, const bool child_can_en_passant) noexcept
    : castle_(move),
      move_type_(MoveType::CASTLE),
      child_castling_rights_(child_castling_rights),
      child_can_en_passant_(child_can_en_passant) {}

Edge::Edge(const QuietPromotion& move, const CastlingRights child_castling_rights,
           const bool child_can_en_passant) noexcept
    : quiet_promotion_(move),
      move_type_(MoveType::QUIET_PROMOTION),
      child_castling_rights_(child_castling_rights),
      child_can_en_passant_(child_can_en_passant) {}

Edge::Edge(const CapturePromotion& move, const CastlingRights child_castling_rights,
           const bool child_can_en_passant) noexcept
    : capture_promotion_(move),
      move_type_(MoveType::CAPTURE_PROMOTION),
      child_castling_rights_(child_castling_rights),
      child_can_en_passant_(child_can_en_passant) {}

Edge::Edge(const EnPassant& move, const CastlingRights child_castling_rights, const bool child_can_en_passant) noexcept
    : en_passant_(move),
      move_type_(MoveType::EN_PASSANT),
      child_castling_rights_(child_castling_rights),
      child_can_en_passant_(child_can_en_passant) {}

Node::Node(const EdgeCount edge_count, const bool is_check) noexcept : edge_count_(edge_count), is_check_(is_check) {}

std::span<Edge> Node::edges() noexcept { return {reinterpret_cast<Edge*>(this + 1), edge_count_}; }

bool Node::is_check() const noexcept { return is_check_; }

Tree::Tree(const Position& position)
    : root_(move_generator::dispatch(
          position, [&]<auto context>(const auto& node) -> decltype(auto) { return expand<context>(node, arena_); })) {}

Node& Tree::root() noexcept { return root_; }
}  // namespace prodigy::mcts
