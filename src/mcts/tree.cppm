module;

#include <atomic>
#include <concepts>
#include <cstdint>
#include <functional>
#include <limits>
#include <span>
#include <utility>

export module prodigy.mcts:tree;

import prodigy.core;

import :arena;

namespace prodigy::mcts {
export class Node;

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

  template <Color side_to_move>
  decltype(auto) visit_move(auto&& visitor) const {
    switch (move_type_) {
#define _(MOVE_TYPE, ...) \
  case MOVE_TYPE:         \
    return std::invoke(std::forward<decltype(visitor)>(visitor), __VA_ARGS__)
      _(MoveType::QUIET_MOVE, quiet_move_);
      _(MoveType::ENABLE_EN_PASSANT, quiet_move_, EnableEnPassant());
      _(MoveType::QUIET_MOVE_NEW_CASTLING_RIGHTS, quiet_move_, child_castling_rights_);
      _(MoveType::CAPTURE, capture_);
      _(MoveType::CAPTURE_NEW_CASTLING_RIGHTS, capture_, child_castling_rights_);
      _(MoveType::KINGSIDE_CASTLE, ColorTraits<side_to_move>::KINGSIDE_CASTLE);
      _(MoveType::QUEENSIDE_CASTLE, ColorTraits<side_to_move>::QUEENSIDE_CASTLE);
      _(MoveType::QUIET_PROMOTION, quiet_promotion_);
      _(MoveType::CAPTURE_PROMOTION, capture_promotion_);
      _(MoveType::CAPTURE_PROMOTION_NEW_CASTLING_RIGHTS, capture_promotion_, child_castling_rights_);
      _(MoveType::EN_PASSANT, en_passant_);
#undef _
    }
  }

  std::pair<Node&, bool> get_or_create_child(std::invocable<> auto&& create_child,
                                             std::invocable<const Node&> auto&& delete_child) {
    auto child = child_.load(std::memory_order_acquire);
    if (child == nullptr) {
      auto& node = std::invoke(std::forward<decltype(create_child)>(create_child));
      if (child_.compare_exchange_strong(child, &node, std::memory_order_acq_rel)) {
        return {node, true};
      }
      std::invoke(std::forward<decltype(delete_child)>(delete_child), node);
    }
    return {*child, false};
  }

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
  std::atomic<Node*> child_ = nullptr;
};
static_assert(sizeof(Edge) == 40);

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
