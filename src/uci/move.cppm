module;

#include <concepts>
#include <iosfwd>
#include <optional>
#include <tuple>
#include <type_traits>

export module prodigy.uci:move;

import prodigy.core;

export namespace prodigy::uci {
struct Move {
  Square origin;
  Square target;
  std::optional<PieceType> promotion;

  friend constexpr auto operator<=>(const Move lhs, const Move rhs) noexcept {
    return std::tie(lhs.origin, lhs.target, lhs.promotion) <=> std::tie(rhs.origin, rhs.target, rhs.promotion);
  }

  friend constexpr bool operator==(Move, Move) = default;
};

constexpr Move to_move(const auto& move) noexcept {
  if constexpr (std::derived_from<std::remove_cvref_t<decltype(move)>, Castle>) {
    return {
        .origin = square_of(move.king_origin),
        .target = square_of(move.king_target),
    };
  } else if constexpr (requires { move.promotion; }) {
    return {
        .origin = square_of(move.origin),
        .target = square_of(move.target),
        .promotion = move.promotion,
    };
  } else {
    return {
        .origin = square_of(move.origin),
        .target = square_of(move.target),
    };
  }
}

std::ostream& operator<<(std::ostream&, Move);
}  // namespace prodigy::uci
