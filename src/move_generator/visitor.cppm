module;

#include <concepts>
#include <utility>

export module prodigy.move_generator:visitor;

import prodigy.core;

import :node;

export namespace prodigy::move_generator {
template <typename Derived>
class Visitor {
 public:
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
  template <std::invocable<> Undo>
  class AutoUndo {
   public:
    constexpr explicit AutoUndo(Undo&& undo) noexcept : undo_(std::forward<Undo>(undo)) {}

    AutoUndo(const AutoUndo&) = delete;
    AutoUndo& operator=(const AutoUndo&) = delete;

    AutoUndo(AutoUndo&&) = delete;
    AutoUndo& operator=(AutoUndo&&) = delete;

    constexpr ~AutoUndo() { std::forward<Undo>(undo_)(); }

   private:
    Undo undo_;
  };

  template <std::invocable<> Undo>
  explicit AutoUndo(Undo&&) -> AutoUndo<Undo>;

 protected:
  template <bool can_en_passant, typename Move>
  static constexpr auto scoped_move(Node& node, const Move& move) noexcept {
    node.board.apply(move);
    if constexpr (can_en_passant) {
      static_assert(std::same_as<Move, QuietMove>);
      return AutoUndo([&, en_passant_victim_origin = std::exchange(node.en_passant_victim_origin, move.target)] {
        node.board.apply(move);
        node.en_passant_victim_origin = en_passant_victim_origin;
      });
    } else {
      return AutoUndo([&] { node.board.apply(move); });
    }
  }
};
}  // namespace prodigy::move_generator
