#include <catch2/catch_test_macros.hpp>
#include <magic_enum.hpp>
#include <utility>

import prodigy.core;

namespace prodigy {
namespace {
using namespace magic_enum;

TEST_CASE("to_bitboard") {
  enum_for_each<Square>([](const auto square) {
    STATIC_REQUIRE(to_bitboard(square) == Bitboard{1ULL << std::to_underlying(square.value)});
  });
}

TEST_CASE("empty") {
  STATIC_REQUIRE(empty(Bitboard()));
  enum_for_each<Square>([](const auto square) { STATIC_REQUIRE_FALSE(empty(to_bitboard(square))); });
}

TEST_CASE("shift") {
  enum_for_each<Square>([](const auto square) {
    static constexpr auto file = file_of(square);
    static constexpr auto rank = rank_of(square);
    static constexpr auto bitboard = to_bitboard(square);
    enum_for_each<Direction>([](const auto direction) {
      switch (direction) {
        case Direction::NORTH:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::NORTH); rank == Rank::EIGHT) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(file, enum_value<Rank, std::to_underlying(rank) + 1>())));
          }
          break;
        case Direction::EAST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::EAST); file == File::H) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(enum_value<File, std::to_underlying(file) + 1>(), rank)));
          }
          break;
        case Direction::SOUTH:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::SOUTH); rank == Rank::ONE) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(file, enum_value<Rank, std::to_underlying(rank) - 1>())));
          }
          break;
        case Direction::WEST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::WEST); file == File::A) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(enum_value<File, std::to_underlying(file) - 1>(), rank)));
          }
          break;
        case Direction::NORTH_EAST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::NORTH_EAST);
                        file == File::H || rank == Rank::EIGHT) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(enum_value<File, std::to_underlying(file) + 1>(),
                                                            enum_value<Rank, std::to_underlying(rank) + 1>())));
          }
          break;
        case Direction::SOUTH_EAST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::SOUTH_EAST);
                        file == File::H || rank == Rank::ONE) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(enum_value<File, std::to_underlying(file) + 1>(),
                                                            enum_value<Rank, std::to_underlying(rank) - 1>())));
          }
          break;
        case Direction::SOUTH_WEST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::SOUTH_WEST);
                        file == File::A || rank == Rank::ONE) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(enum_value<File, std::to_underlying(file) - 1>(),
                                                            enum_value<Rank, std::to_underlying(rank) - 1>())));
          }
          break;
        case Direction::NORTH_WEST:
          if constexpr (static constexpr auto shifted = shift(bitboard, Direction::NORTH_WEST);
                        file == File::A || rank == Rank::EIGHT) {
            STATIC_REQUIRE(empty(shifted));
          } else {
            STATIC_REQUIRE(shifted == to_bitboard(to_square(enum_value<File, std::to_underlying(file) - 1>(),
                                                            enum_value<Rank, std::to_underlying(rank) + 1>())));
          }
          break;
      }
    });
  });
}
}  // namespace
}  // namespace prodigy
