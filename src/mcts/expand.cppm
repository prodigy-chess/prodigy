module;

#include <cassert>
#include <concepts>
#include <limits>
#include <utility>

module prodigy.mcts:expand;

import prodigy.core;
import prodigy.move_generator;

import :arena;
import :tree;

namespace prodigy::mcts {
template <move_generator::Node::Context parent_context>
class EdgeInserter : public move_generator::Visitor<EdgeInserter<parent_context>> {
 public:
  explicit EdgeInserter(EdgeCount& edge_count, bool& is_check, Arena& arena) noexcept
      : edge_count_(edge_count), is_check_(is_check), arena_(arena) {}

  template <move_generator::Node::Context child_context>
  void visit_pawn_move(const auto& move) const noexcept {
    insert<child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_knight_move(const auto& move) const noexcept {
    insert<child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_bishop_move(const auto& move) const noexcept {
    insert<child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_rook_move(const auto& move) const noexcept {
    insert<child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_queen_move(const auto& move) const noexcept {
    insert<child_context>(move);
  }

  template <move_generator::Node::Context child_context>
  void visit_king_move(const auto& move) const noexcept {
    insert<child_context>(move);
  }

  void is_check() const noexcept { is_check_ = true; }

 private:
  void insert(auto&&... args) const noexcept {
    assert(edge_count_ < std::numeric_limits<EdgeCount>::max());
    ++edge_count_;
    arena_.new_object<Edge>(std::forward<decltype(args)>(args)...);
  }

  template <move_generator::Node::Context child_context>
    requires(parent_context.enable_en_passant() == child_context)
  void insert(const auto& move) const noexcept {
    insert(move, Edge::EnableEnPassant());
  }

  template <move_generator::Node::Context child_context, typename Move>
    requires(parent_context.move(parent_context.castling_rights) == child_context || std::derived_from<Move, Castle>)
  void insert(const Move& move) const noexcept {
    insert(move);
  }

  template <move_generator::Node::Context child_context>
  void insert(const auto& move) const noexcept {
    insert(move, child_context.castling_rights);
  }

  EdgeCount& edge_count_;
  bool& is_check_;
  Arena& arena_;
};

template <move_generator::Node::Context context>
Node& expand(const move_generator::Node& node, Arena& arena) noexcept {
  EdgeCount edge_count = 0;
  bool is_check = false;
  move_generator::walk<context>(node, EdgeInserter<context>(edge_count, is_check, arena));
  return arena.new_object<Node>(edge_count, is_check);
}
}  // namespace prodigy::mcts
