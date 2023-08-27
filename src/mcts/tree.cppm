module;

#include <cstdint>
#include <limits>
#include <span>

export module prodigy.mcts:tree;

import prodigy.core;

import :arena;

namespace prodigy::mcts {
export class alignas(Arena::ALIGNMENT) Edge {
 public:
  struct EnableEnPassant final {};

  explicit Edge(const QuietMove&) noexcept;

  explicit Edge(const QuietMove&, EnableEnPassant) noexcept;

  explicit Edge(const QuietMove&, CastlingRights child_castling_rights) noexcept;

  explicit Edge(const Capture&) noexcept;

  explicit Edge(const Capture&, CastlingRights child_castling_rights) noexcept;

  explicit Edge(const KingsideCastle&) noexcept;

  explicit Edge(const QueensideCastle&) noexcept;

  explicit Edge(const QuietPromotion&) noexcept;

  explicit Edge(const CapturePromotion&) noexcept;

  explicit Edge(const CapturePromotion&, CastlingRights child_castling_rights) noexcept;

  explicit Edge(const EnPassant&) noexcept;

  Edge(const Edge&) = delete;
  Edge& operator=(const Edge&) = delete;

  Edge(Edge&&) = delete;
  Edge& operator=(Edge&&) = delete;

 private:
  enum class MoveType : std::uint8_t {
    QUIET_MOVE,
    ENABLE_EN_PASSANT,
    QUIET_MOVE_NEW_CASTLING_RIGHTS,
    CAPTURE,
    CAPTURE_NEW_CASTLING_RIGHTS,
    KINGSIDE_CASTLE,
    QUEENSIDE_CASTLE,
    QUIET_PROMOTION,
    CAPTURE_PROMOTION,
    CAPTURE_PROMOTION_NEW_CASTLING_RIGHTS,
    EN_PASSANT,
  };

  union {
    const QuietMove quiet_move_;
    const Capture capture_;
    const QuietPromotion quiet_promotion_;
    const CapturePromotion capture_promotion_;
    const EnPassant en_passant_;
  };
  const MoveType move_type_;
  CastlingRights child_castling_rights_;
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
