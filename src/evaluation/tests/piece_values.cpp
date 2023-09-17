#include <catch2/catch_test_macros.hpp>
#include <magic_enum_utility.hpp>

import prodigy.core;
import prodigy.evaluation;

namespace prodigy::evaluation {
namespace {
using namespace magic_enum;

TEST_CASE("piece values") {
  enum_for_each<Phase>([](const auto phase) {
    static constexpr auto& piece_values = PIECE_VALUES[phase];
    enum_for_each<PieceType>([](const auto piece_type) {
      if constexpr (piece_type.value == PieceType::KING) {
        STATIC_REQUIRE(piece_values[piece_type] == 0);
      } else if constexpr (static constexpr auto prev_piece_type = enum_prev_value(piece_type.value);
                           prev_piece_type.has_value()) {
        STATIC_REQUIRE(piece_values[*prev_piece_type] < piece_values[piece_type]);
      }
    });
  });
}
}  // namespace
}  // namespace prodigy::evaluation
