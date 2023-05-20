#include <catch2/catch_test_macros.hpp>

import prodigy.core;

namespace prodigy {
namespace {
TEST_CASE("logical NOT") {
  STATIC_REQUIRE(!Color::WHITE == Color::BLACK);
  STATIC_REQUIRE(!Color::BLACK == Color::WHITE);
}
}  // namespace
}  // namespace prodigy
