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
  template <Node::Context context>
  constexpr void visit_pawn_move(const QuietMove& double_push, const Bitboard en_passant_target) const noexcept {
    static_cast<const Derived*>(this)->template visit_pawn_move<context>(double_push, en_passant_target);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_pawn_move(const Move& move) const noexcept {
    static_cast<const Derived*>(this)->template visit_pawn_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_knight_move(const Move& move) const noexcept {
    static_cast<const Derived*>(this)->template visit_knight_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_bishop_move(const Move& move) const noexcept {
    static_cast<const Derived*>(this)->template visit_bishop_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_rook_move(const Move& move) const noexcept {
    static_cast<const Derived*>(this)->template visit_rook_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_queen_move(const Move& move) const noexcept {
    static_cast<const Derived*>(this)->template visit_queen_move<context>(move);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_king_move(const Move& move) const noexcept {
    static_cast<const Derived*>(this)->template visit_king_move<context>(move);
  }

 private:
  template <std::invocable<> Undo>
  class AutoUndo {
   public:
    constexpr explicit AutoUndo(Undo&& undo) : undo_(std::forward<Undo>(undo)) { std::forward<Undo>(undo_)(); }

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
  template <typename Move>
  static constexpr auto scoped_move(Node& node, const Move& move,
                                    const Bitboard en_passant_target = Bitboard()) noexcept {
    return AutoUndo([&] {
      node.board.apply(move);
      node.en_passant_target ^= en_passant_target;
    });
  }
};
}  // namespace prodigy::move_generator
