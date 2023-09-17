#include <catch2/catch_test_macros.hpp>
#include <magic_enum_utility.hpp>
#include <utility>

import prodigy.core;
import prodigy.evaluation;

namespace prodigy::evaluation {
namespace {
using namespace magic_enum;

consteval Square vertical_flip(const Square square) noexcept {
  return static_cast<Square>(std::to_underlying(square) ^ 56);
}

TEST_CASE("piece square tables") {
  enum_for_each<PieceType>([](const auto piece_type) {
    static constexpr auto& midgame_psqt = MIDGAME_PSQT[piece_type];
    static constexpr auto& endgame_psqt = ENDGAME_PSQT[piece_type];
    enum_for_each<Square>([](const auto square) {
      static constexpr auto vertically_flipped = vertical_flip(square);
      STATIC_REQUIRE(file_of(vertically_flipped) == file_of(square));
      STATIC_REQUIRE(rank_of(vertically_flipped) != rank_of(square));
      STATIC_REQUIRE(vertical_flip(vertically_flipped) == square);
      STATIC_REQUIRE(midgame_psqt[Color::WHITE][square] == midgame_psqt[Color::BLACK][vertically_flipped]);
      STATIC_REQUIRE(endgame_psqt[Color::WHITE][square] == endgame_psqt[Color::BLACK][vertically_flipped]);
    });
  });
}
}  // namespace
}  // namespace prodigy::evaluation
