module prodigy.mcts:expand;

import prodigy.move_generator;

import :arena;
import :tree;

namespace prodigy::mcts {
class EdgeInserter : public move_generator::Visitor<EdgeInserter> {
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
  template <move_generator::Node::Context child_context>
  void insert(const auto& move) const noexcept {
    ++edge_count_;
    arena_.new_object<Edge>(move, child_context.castling_rights, child_context.can_en_passant);
  }

  EdgeCount& edge_count_;
  bool& is_check_;
  Arena& arena_;
};

template <move_generator::Node::Context context>
Node& expand(const move_generator::Node& node, Arena& arena) noexcept {
  EdgeCount edge_count = 0;
  bool is_check = false;
  move_generator::walk<context>(node, EdgeInserter(edge_count, is_check, arena));
  return arena.new_object<Node>(edge_count, is_check);
}
}  // namespace prodigy::mcts
