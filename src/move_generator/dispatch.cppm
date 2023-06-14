module;

#include <utility>

export module prodigy.move_generator:dispatch;

import prodigy.core;

import :node;

export namespace prodigy::move_generator {
template <typename Callback>
constexpr decltype(auto) dispatch(const Position& position, Callback&& callback) {
#define HANDLE(value, ...) std::forward<decltype(callback)>(callback).template operator()<value>(__VA_ARGS__)
  const auto dispatch_side_to_move = [&](auto&& callback) -> decltype(auto) {
    switch (position.side_to_move) {
      case Color::WHITE:
        return HANDLE(Color::WHITE);
      case Color::BLACK:
        return HANDLE(Color::BLACK);
    }
  };
  const auto dispatch_castling_rights = [&](auto&& callback) -> decltype(auto) {
    switch (std::to_underlying(position.castling_rights)) {
#define _(CASTLING_RIGHTS) \
  case CASTLING_RIGHTS:    \
    return HANDLE(CastlingRights{CASTLING_RIGHTS})
      _(0);
      _(1);
      _(2);
      _(3);
      _(4);
      _(5);
      _(6);
      _(7);
      _(8);
      _(9);
      _(10);
      _(11);
      _(12);
      _(13);
      _(14);
      _(15);
#undef _
      default:
        std::unreachable();
    }
  };
  const auto dispatch_en_passant_target = [&](auto&& callback) -> decltype(auto) {
    return position.en_passant_target.has_value() ? HANDLE(true, to_bitboard(*position.en_passant_target))
                                                  : HANDLE(false, Bitboard());
  };
  return dispatch_side_to_move([&]<auto side_to_move>->decltype(auto) {
    return dispatch_castling_rights([&]<auto castling_rights>->decltype(auto) {
      return dispatch_en_passant_target([&]<auto has_en_passant_target>(const auto en_passant_target)->decltype(auto) {
        return HANDLE((Node::Context{side_to_move, castling_rights, has_en_passant_target}),
                      Node{position.board, en_passant_target});
      });
    });
  });
#undef HANDLE
}
}  // namespace prodigy::move_generator
