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
}  // namespace
}  // namespace prodigy
