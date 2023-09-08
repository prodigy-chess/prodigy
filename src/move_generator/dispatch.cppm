module;

#include <utility>

export module prodigy.move_generator:dispatch;

import prodigy.core;

import :lookup;
import :node;

export namespace prodigy::move_generator {
constexpr decltype(auto) dispatch(const Position& position, auto&& callback) {
#define HANDLE(value, ...) std::forward<decltype(callback)>(callback).template operator()<value>(__VA_ARGS__)
  const auto dispatch_side_to_move = [&](auto&& callback) -> decltype(auto) {
    switch (position.side_to_move) {
      case Color::WHITE:
        return HANDLE(Color::WHITE);
      case Color::BLACK:
        return HANDLE(Color::BLACK);
    }
  };
  const auto dispatch_can_en_passant = [&]<auto side_to_move>(auto&& callback) -> decltype(auto) {
    return position.en_passant_target.has_value()
               ? HANDLE(true, pawn_single_push_set(!side_to_move, to_bitboard(*position.en_passant_target)))
               : HANDLE(false, Bitboard());
  };
  return dispatch_side_to_move([&]<auto side_to_move> -> decltype(auto) {
    return dispatch(position.castling_rights, [&]<auto castling_rights> -> decltype(auto) {
      return dispatch_can_en_passant.template operator()<side_to_move>(
          [&]<auto can_en_passant>(const auto en_passant_victim_origin) -> decltype(auto) {
            return HANDLE((Node::Context{side_to_move, castling_rights, can_en_passant}),
                          Node{position.board, en_passant_victim_origin});
          });
    });
  });
#undef HANDLE
}
}  // namespace prodigy::move_generator
