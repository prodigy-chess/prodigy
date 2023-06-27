module;

#include <utility>

export module prodigy.move_generator:visitor;

import prodigy.core;

import :node;

export namespace prodigy::move_generator {
template <typename Derived>
class Visitor {
 public:
  template <Node::Context context>
  constexpr void visit_pawn_move(const QuietMove& double_push, const Bitboard en_passant_target) {
    static_cast<Derived*>(this)->template visit_pawn_move<context>(double_push, en_passant_target);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_pawn_move(const Move& move) {
    static_cast<Derived*>(this)->template visit_pawn_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_knight_move(const Move& move) {
    static_cast<Derived*>(this)->template visit_knight_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_bishop_move(const Move& move) {
    static_cast<Derived*>(this)->template visit_bishop_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_rook_move(const Move& move) {
    static_cast<Derived*>(this)->template visit_rook_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_queen_move(const Move& move) {
    static_cast<Derived*>(this)->template visit_queen_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_king_move(const Move& move) {
    static_cast<Derived*>(this)->template visit_king_move<context>(move);
  }

 private:
  template <typename Move>
  class AutoUndo {
   public:
    constexpr explicit AutoUndo(Node& node, const Move& move, const Bitboard en_passant_target) noexcept
        : node_(node), move_(move), en_passant_target_(en_passant_target) {}

    AutoUndo(const AutoUndo&) = delete;
    AutoUndo& operator=(const AutoUndo&) = delete;

    AutoUndo(AutoUndo&&) = delete;
    AutoUndo& operator=(AutoUndo&&) = delete;

    constexpr ~AutoUndo() {
      node_.board.apply(move_);
      node_.en_passant_target = en_passant_target_;
    }

   private:
    Node& node_;
    const Move& move_;
    Bitboard en_passant_target_;
  };

 protected:
  template <typename Move>
  static constexpr AutoUndo<Move> scoped_move(Node& node, const Move& move, const Bitboard en_passant_target) noexcept {
    node.board.apply(move);
    return AutoUndo<Move>(node, move, std::exchange(node.en_passant_target, en_passant_target));
  }
};
}  // namespace prodigy::move_generator
