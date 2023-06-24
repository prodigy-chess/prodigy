module;

#include <bit>
#include <concepts>
#include <cstdint>
#include <functional>
#include <magic_enum_utility.hpp>
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
  return static_cast<Square>(std::countr_zero(std::to_underlying(bitboard)));
}

constexpr int popcount(const Bitboard bitboard) noexcept { return std::popcount(std::to_underlying(bitboard)); }

constexpr Bitboard unsafe_shift(const Bitboard bitboard, const Direction direction) noexcept {
  switch (const auto shift = std::to_underlying(direction); direction) {
    case Direction::NORTH:
    case Direction::EAST:
    case Direction::NORTH_EAST:
    case Direction::NORTH_WEST:
      return Bitboard{std::to_underlying(bitboard) << shift};
    case Direction::SOUTH:
    case Direction::WEST:
    case Direction::SOUTH_EAST:
    case Direction::SOUTH_WEST:
      return Bitboard{std::to_underlying(bitboard) >> -shift};
  }
}

constexpr Bitboard shift(const Bitboard bitboard, const Direction direction) noexcept {
  static constexpr auto off_mask = [](const auto file) consteval {
    Bitboard off_mask{};
    magic_enum::enum_for_each<Rank>([&](const auto rank) { off_mask |= to_bitboard(to_square(file, rank)); });
    return ~off_mask;
  };
  switch (const auto shifted = unsafe_shift(bitboard, direction); direction) {
    case Direction::NORTH:
    case Direction::SOUTH:
      return shifted;
    case Direction::EAST:
    case Direction::NORTH_EAST:
    case Direction::SOUTH_EAST:
      return shifted & off_mask(File::A);
    case Direction::WEST:
    case Direction::SOUTH_WEST:
    case Direction::NORTH_WEST:
      return shifted & off_mask(File::H);
  }
}

template <std::invocable<Bitboard> Callback>
constexpr void for_each_bit(Bitboard bitboard, Callback&& callback) {
  while (any(bitboard)) {
    const auto lsb = bitboard & Bitboard{-std::to_underlying(bitboard)};
    std::invoke(std::forward<Callback>(callback), lsb);
    bitboard ^= lsb;
  }
}

template <std::invocable<Square> Callback>
constexpr void for_each_square(Bitboard bitboard, Callback&& callback) {
  for (; any(bitboard); bitboard &= Bitboard{std::to_underlying(bitboard) - 1}) {
    std::invoke(std::forward<Callback>(callback), square_of(bitboard));
  }
}

template <std::invocable<Bitboard, Square> Callback>
constexpr void for_each_bit_and_square(const Bitboard bitboard, Callback&& callback) {
  for_each_square(
      bitboard, [&](const auto square) { std::invoke(std::forward<Callback>(callback), to_bitboard(square), square); });
}
}  // namespace prodigy
