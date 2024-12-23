#include <catch2/catch_test_macros.hpp>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_utility.hpp>
#include <set>
#include <utility>

import prodigy.core;

namespace prodigy {
namespace {
using namespace magic_enum;

TEST_CASE("square") {
  enum_for_each<Square>([](const auto square) {
    static constexpr auto bitboard = to_bitboard(square);
    STATIC_REQUIRE(bitboard == Bitboard{1ULL << std::to_underlying(square.value)});
    STATIC_REQUIRE(square_of(bitboard) == square);
  });
}

TEST_CASE("any") {
  STATIC_REQUIRE_FALSE(any(Bitboard()));
  enum_for_each<Square>([](const auto square) { STATIC_REQUIRE(any(to_bitboard(square))); });
}

TEST_CASE("empty") {
  STATIC_REQUIRE(empty(Bitboard()));
  enum_for_each<Square>([](const auto square) { STATIC_REQUIRE_FALSE(empty(to_bitboard(square))); });
}

TEST_CASE("popcount") {
  STATIC_REQUIRE(popcount(Bitboard()) == 0);
  STATIC_REQUIRE(popcount(~Bitboard()) == enum_count<Square>());
  enum_for_each<Square>([](const auto square) { STATIC_REQUIRE(popcount(to_bitboard(square)) == 1); });
}

TEST_CASE("shift") {
  enum_for_each<Direction>([](const auto direction) {
    STATIC_REQUIRE(empty(shift(Bitboard(), direction)));
    STATIC_REQUIRE(empty(unsafe_shift(Bitboard(), direction)));
    enum_for_each<Square>([&](const auto square) {
      static constexpr auto file = file_of(square);
      static constexpr auto rank = rank_of(square);
      static constexpr auto bitboard = to_bitboard(square);
      switch (direction) {
        case Direction::NORTH:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::NORTH); rank == Rank::EIGHT) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(file, *enum_next_value(rank))));
            STATIC_REQUIRE(unsafe_shift(shifted, Direction::SOUTH) == bitboard);
          }
          break;
        case Direction::EAST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::EAST); file == File::H) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(*enum_next_value(file), rank)));
            STATIC_REQUIRE(unsafe_shift(shifted, Direction::WEST) == bitboard);
          }
          break;
        case Direction::SOUTH:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::SOUTH); rank == Rank::ONE) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(file, *enum_prev_value(rank))));
            STATIC_REQUIRE(unsafe_shift(shifted, Direction::NORTH) == bitboard);
          }
          break;
        case Direction::WEST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::WEST); file == File::A) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(*enum_prev_value(file), rank)));
            STATIC_REQUIRE(unsafe_shift(shifted, Direction::EAST) == bitboard);
          }
          break;
        case Direction::NORTH_EAST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::NORTH_EAST);
                        file == File::H || rank == Rank::EIGHT) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(*enum_next_value(file), *enum_next_value(rank))));
            STATIC_REQUIRE(unsafe_shift(shifted, Direction::SOUTH_WEST) == bitboard);
          }
          break;
        case Direction::SOUTH_EAST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::SOUTH_EAST);
                        file == File::H || rank == Rank::ONE) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(*enum_next_value(file), *enum_prev_value(rank))));
            STATIC_REQUIRE(unsafe_shift(shifted, Direction::NORTH_WEST) == bitboard);
          }
          break;
        case Direction::SOUTH_WEST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::SOUTH_WEST);
                        file == File::A || rank == Rank::ONE) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(*enum_prev_value(file), *enum_prev_value(rank))));
            STATIC_REQUIRE(unsafe_shift(shifted, Direction::NORTH_EAST) == bitboard);
          }
          break;
        case Direction::NORTH_WEST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::NORTH_WEST);
                        file == File::A || rank == Rank::EIGHT) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(*enum_prev_value(file), *enum_next_value(rank))));
            STATIC_REQUIRE(unsafe_shift(shifted, Direction::SOUTH_EAST) == bitboard);
          }
          break;
      }
    });
  });
}

TEST_CASE("for_each") {
  static constexpr auto no_bits = Bitboard();
  static constexpr auto all_bits = ~no_bits;

  for_each_bit(no_bits, [](auto) { FAIL(); });
  Bitboard bits{};
  for_each_bit(all_bits, [&](const auto bit) {
    REQUIRE(empty(bits & bit));
    bits |= bit;
  });
  REQUIRE(bits == all_bits);

  for_each_square(no_bits, [](auto) { FAIL(); });
  std::set<Square> squares;
  for_each_square(all_bits, [&](const auto square) { REQUIRE(squares.emplace(square).second); });
  REQUIRE(squares.size() == enum_count<Square>());

  for_each_bit_and_square(no_bits, [](auto...) { FAIL(); });
  for_each_bit_and_square(all_bits, [&](const auto bit, const auto square) {
    REQUIRE(any(bits & bit));
    bits ^= bit;
    REQUIRE(squares.erase(square));
  });
  REQUIRE(empty(bits));
  REQUIRE(squares.empty());
}
}  // namespace
}  // namespace prodigy
