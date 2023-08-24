module;

#include <cstdint>
#include <functional>
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

export class alignas(Arena::ALIGNMENT) Node {
 public:
  explicit Node(std::uint8_t edge_count, bool is_check) noexcept;

  std::span<Edge> edges() noexcept;

  bool is_check() const noexcept;

 private:
  const std::uint8_t edge_count_;
  const bool is_check_;
};
static_assert(sizeof(Node) == 8);
}  // namespace prodigy::mcts
