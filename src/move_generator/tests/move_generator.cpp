#include <catch2/catch_test_macros.hpp>

import prodigy.move_generator;

namespace prodigy::move_generator {
namespace {
TEST_CASE("init") {
  REQUIRE(init().has_value());
  const auto result = init();
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() == "Already initialized.");
}
}  // namespace
}  // namespace prodigy::move_generator
