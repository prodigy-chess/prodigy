module;

#include <cstdint>
#include <functional>
#include <limits>
#include <span>

export module prodigy.mcts:tree;

import prodigy.core;

import :arena;

namespace prodigy::mcts {
export class alignas(Arena::ALIGNMENT) Edge {
 public:
  explicit Edge(const QuietMove&, CastlingRights child_castling_rights, bool child_can_en_passant) noexcept;

  explicit Edge(const Capture&, CastlingRights child_castling_rights, bool child_can_en_passant) noexcept;

  explicit Edge(const Castle&, CastlingRights child_castling_rights, bool child_can_en_passant) noexcept;

  explicit Edge(const QuietPromotion&, CastlingRights child_castling_rights, bool child_can_en_passant) noexcept;

  explicit Edge(const CapturePromotion&, CastlingRights child_castling_rights, bool child_can_en_passant) noexcept;

  explicit Edge(const EnPassant&, CastlingRights child_castling_rights, bool child_can_en_passant) noexcept;

  Edge(const Edge&) = delete;
  Edge& operator=(const Edge&) = delete;

  Edge(Edge&&) = delete;
  Edge& operator=(Edge&&) = delete;

 private:
  enum class MoveType : std::uint8_t { QUIET_MOVE, CAPTURE, CASTLE, QUIET_PROMOTION, CAPTURE_PROMOTION, EN_PASSANT };

  union {
    const QuietMove quiet_move_;
    const Capture capture_;
    const std::reference_wrapper<const Castle> castle_;
    const QuietPromotion quiet_promotion_;
    const CapturePromotion capture_promotion_;
    const EnPassant en_passant_;
  };
  const MoveType move_type_;
  const CastlingRights child_castling_rights_;
  const bool child_can_en_passant_;
};
static_assert(sizeof(Edge) == 32);

using EdgeCount = std::uint8_t;

export class alignas(Arena::ALIGNMENT) Node {
 public:
  explicit Node(EdgeCount, bool is_check) noexcept;

  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;

  Node(Node&&) = delete;
  Node& operator=(Node&&) = delete;

  std::span<Edge> edges() noexcept;

  bool is_check() const noexcept;

 private:
  const EdgeCount edge_count_;
  const bool is_check_;
};
static_assert(sizeof(Node) == 8);

export class Tree {
 public:
  explicit Tree(const Position&);

  Node& root() noexcept;

 private:
  Arena arena_{sizeof(Node) + sizeof(Edge) * std::numeric_limits<EdgeCount>::max()};
  Node& root_;
};
}  // namespace prodigy::mcts
