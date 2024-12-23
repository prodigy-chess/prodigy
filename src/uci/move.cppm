module;

#include <concepts>
#include <expected>
#include <iosfwd>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <string_view>
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

[[nodiscard]] constexpr std::expected<Move, std::string_view> parse_move(const std::string_view move) noexcept {
  if (move.size() < 4) {
    return std::unexpected("Too few characters.");
  }
  if (move.size() > 5) {
    return std::unexpected("Too many characters.");
  }
  const auto origin = magic_enum::enum_cast<Square>(move.substr(0, 2), magic_enum::case_insensitive);
  if (!origin.has_value()) {
    return std::unexpected("Invalid origin.");
  }
  const auto target = magic_enum::enum_cast<Square>(move.substr(2, 2), magic_enum::case_insensitive);
  if (!target.has_value()) {
    return std::unexpected("Invalid target.");
  }
  std::optional<PieceType> promotion;
  if (move.size() == 5) {
    switch (move.back()) {
      case 'n':
        promotion = PieceType::KNIGHT;
        break;
      case 'b':
        promotion = PieceType::BISHOP;
        break;
      case 'r':
        promotion = PieceType::ROOK;
        break;
      case 'q':
        promotion = PieceType::QUEEN;
        break;
      default:
        return std::unexpected("Invalid promotion.");
    }
  }
  return Move{
      .origin = *origin,
      .target = *target,
      .promotion = promotion,
  };
}

std::ostream& operator<<(std::ostream&, Move);
}  // namespace prodigy::uci
