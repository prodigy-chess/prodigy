#include <catch2/catch_test_macros.hpp>

import prodigy.chess;

namespace prodigy::chess {
namespace {
TEST_CASE("bitwise NOT") {
  STATIC_REQUIRE(~Color::WHITE == Color::BLACK);
  STATIC_REQUIRE(~Color::BLACK == Color::WHITE);
}
}  // namespace
}  // namespace prodigy::chess
