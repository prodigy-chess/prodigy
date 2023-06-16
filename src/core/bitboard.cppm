module;

#include <bit>
#include <cassert>
#include <cstdint>
#include <magic_enum.hpp>
#include <utility>

#include "macros.h"

export module prodigy.core:bitboard;

import :direction;
import :square;

export namespace prodigy {
enum class Bitboard : std::uint64_t {};

PRODIGY_ENUM_BITWISE_OPERATORS(Bitboard)

constexpr Bitboard to_bitboard(const Square square) noexcept { return Bitboard{1ULL << std::to_underlying(square)}; }

constexpr Square square_of(const Bitboard bitboard) noexcept {
  assert(std::has_single_bit(std::to_underlying(bitboard)));
  return static_cast<Square>(std::countr_zero(std::to_underlying(bitboard)));
}

constexpr bool any(const Bitboard bitboard) noexcept { return static_cast<bool>(bitboard); }

constexpr int popcount(const Bitboard bitboard) noexcept { return std::popcount(std::to_underlying(bitboard)); }

constexpr Bitboard shift(const Bitboard bitboard, const Direction direction) noexcept {
  static constexpr auto off_mask = [](const auto file) consteval {
    Bitboard off_mask{};
    magic_enum::enum_for_each<Rank>([&](const auto rank) { off_mask |= to_bitboard(to_square(file, rank)); });
    return ~off_mask;
  };
  switch (const auto shift = std::to_underlying(direction); direction) {
    case Direction::NORTH:
      return Bitboard{std::to_underlying(bitboard) << shift};
    case Direction::EAST:
    case Direction::NORTH_EAST:
      return Bitboard{std::to_underlying(bitboard & off_mask(File::H)) << shift};
    case Direction::SOUTH:
      return Bitboard{std::to_underlying(bitboard) >> -shift};
    case Direction::WEST:
    case Direction::SOUTH_WEST:
      return Bitboard{std::to_underlying(bitboard & off_mask(File::A)) >> -shift};
    case Direction::SOUTH_EAST:
      return Bitboard{std::to_underlying(bitboard & off_mask(File::H)) >> -shift};
    case Direction::NORTH_WEST:
      return Bitboard{std::to_underlying(bitboard & off_mask(File::A)) << shift};
  }
}
}  // namespace prodigy
