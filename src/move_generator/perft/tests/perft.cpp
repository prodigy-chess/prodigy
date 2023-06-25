#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <string_view>

import prodigy.core;
import prodigy.move_generator.perft;

namespace prodigy::move_generator::perft {
namespace {
TEST_CASE("invalid") {
  const auto [fen, depth, error] = GENERATE(table<std::string_view, Ply, std::string_view>({
      {STARTING_FEN, Ply{0}, "Invalid depth."},
      {STARTING_FEN, Ply{9}, "Invalid depth."},
  }));
  INFO(fen);
  const auto result = run(parse_fen(fen).value(), depth);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() == error);
}
}  // namespace
}  // namespace prodigy::move_generator::perft
