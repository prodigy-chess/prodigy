module;

#include <concepts>
#include <iosfwd>
#include <optional>
#include <tuple>

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

  friend consteval bool operator==(Move, Move) = default;
};

template <typename T>
constexpr Move to_move(const T& value) noexcept {
  Move move;
  if constexpr (std::same_as<T, Castle>) {
    move.origin = square_of(value.king_origin);
    move.target = square_of(value.king_target);
  } else {
    move.origin = square_of(value.origin);
    move.target = square_of(value.target);
  }
  if constexpr (requires { value.promotion; }) {
    move.promotion = value.promotion;
  }
  return move;
}

std::ostream& operator<<(std::ostream&, Move);
}  // namespace prodigy::uci
