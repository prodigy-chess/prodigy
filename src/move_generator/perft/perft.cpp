module;

#include <array>
#include <cstdint>
#include <expected>
#include <string_view>
#include <utility>

module prodigy.move_generator.perft;

import prodigy.core;
import prodigy.move_generator;

namespace prodigy::move_generator::perft {
namespace {
template <Ply depth>
class Visitor : public move_generator::Visitor<Visitor<depth>> {
 public:
  constexpr explicit Visitor(Node& node, std::uint64_t& leaf_node_count) noexcept
      : node_(node), leaf_node_count_(leaf_node_count) {}

  template <Node::Context context>
  constexpr void visit_pawn_move(const QuietMove& double_push, const Bitboard en_passant_target) const noexcept {
    visit<context>(double_push, en_passant_target);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_pawn_move(const Move& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_knight_move(const Move& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_bishop_move(const Move& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_rook_move(const Move& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_queen_move(const Move& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_king_move(const Move& move) const noexcept {
    visit<context>(move);
  }

 private:
  template <Node::Context context, typename Move>
  constexpr void visit(const Move& move, const Bitboard en_passant_target = Bitboard()) const noexcept {
    if constexpr (depth == Ply{0}) {
      ++leaf_node_count_;
    } else {
      const auto undo = this->scoped_move(node_, move, en_passant_target);
      walk<context>(node_, Visitor<Ply{std::to_underlying(depth) - 1}>(node_, leaf_node_count_));
    }
  }

  Node& node_;
  std::uint64_t& leaf_node_count_;
};
}  // namespace

std::expected<std::uint64_t, std::string_view> perft(const Position& position, const Ply depth) {
  switch (std::to_underlying(depth)) {
#define _(DEPTH)                                                           \
  case DEPTH:                                                              \
    return dispatch(position, []<auto context>(auto node) {                \
      std::uint64_t leaf_node_count = 0;                                   \
      walk<context>(node, Visitor<Ply{DEPTH - 1}>(node, leaf_node_count)); \
      return leaf_node_count;                                              \
    })
    _(1);
    _(2);
    _(3);
    _(4);
    _(5);
    _(6);
    _(7);
    _(8);
#undef _
    default:
      return std::unexpected("Invalid depth.");
  }
}
}  // namespace prodigy::move_generator::perft
