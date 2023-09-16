#include <catch2/catch_test_macros.hpp>
#include <magic_enum_utility.hpp>

import prodigy.core;
import prodigy.evaluation;

namespace prodigy::evaluation {
namespace {
using namespace magic_enum;

TEST_CASE("piece values") {
  enum_for_each<PieceType>([](const auto piece_type) {
    if constexpr (piece_type.value == PieceType::KING) {
      STATIC_REQUIRE(MIDGAME_PIECE_VALUES[piece_type] == 0);
      STATIC_REQUIRE(ENDGAME_PIECE_VALUES[piece_type] == 0);
    } else {
      STATIC_REQUIRE(MIDGAME_PIECE_VALUES[piece_type] != ENDGAME_PIECE_VALUES[piece_type]);
      if constexpr (static constexpr auto prev_piece_type = enum_prev_value(piece_type.value);
                    prev_piece_type.has_value()) {
        STATIC_REQUIRE(MIDGAME_PIECE_VALUES[*prev_piece_type] < MIDGAME_PIECE_VALUES[piece_type]);
        STATIC_REQUIRE(ENDGAME_PIECE_VALUES[*prev_piece_type] < ENDGAME_PIECE_VALUES[piece_type]);
      }
    }
  });
}
}  // namespace
}  // namespace prodigy::evaluation
