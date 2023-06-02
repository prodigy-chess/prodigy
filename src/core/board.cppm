module;

#include <magic_enum.hpp>
#include <optional>
#include <utility>

export module prodigy.core:board;

import :bitboard;
import :color;
import :containers;
import :piece_type;
import :square;

export namespace prodigy {
class Board {
 public:
  constexpr Board() = default;

  constexpr explicit Board(
      const EnumMap<Square, std::optional<std::pair<Color, PieceType>>>& piece_placement) noexcept {
    magic_enum::enum_for_each<Square>([&](const auto square) {
      if (const auto piece = piece_placement[square]; piece.has_value()) {
        occupancy_ |= colors_[piece->first] |= pieces_[piece->first][piece->second] |= to_bitboard(square);
      }
    });
  }

  constexpr Bitboard operator[](const Color color, const PieceType piece_type) const noexcept {
    return pieces_[color][piece_type];
  }

  constexpr Bitboard operator[](const Color color) const noexcept { return colors_[color]; }

  constexpr Bitboard occupancy() const noexcept { return occupancy_; }

  friend constexpr bool operator==(const Board&, const Board&) = default;

 private:
  EnumMap<Color, EnumMap<PieceType, Bitboard>> pieces_{};
  EnumMap<Color, Bitboard> colors_{};
  Bitboard occupancy_{};
};
}  // namespace prodigy
