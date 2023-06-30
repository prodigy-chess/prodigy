#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstdint>
#include <string_view>
#include <utility>

import prodigy.core;
import prodigy.move_generator;
import prodigy.move_generator.perft;

namespace prodigy::move_generator::perft {
namespace {
TEST_CASE("perft") {
  static constexpr auto KIWIPETE = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
#ifdef NDEBUG
  const auto [fen, depth, leaf_node_count] = GENERATE(table<std::string_view, Ply, std::uint64_t>({
      {STARTING_FEN, Ply{7}, 3'195'901'860},
      {KIWIPETE, Ply{6}, 8'031'647'685},
  }));
#else
  const auto [fen, depth, leaf_node_count] = GENERATE(table<std::string_view, Ply, std::uint64_t>({
      {STARTING_FEN, Ply{5}, 4'865'609},
      {KIWIPETE, Ply{5}, 193'690'690},
  }));
#endif
  static_cast<void>(init());
  INFO(fen);
  INFO("depth " << std::to_underlying(depth));
  REQUIRE(perft(parse_fen(fen).value(), depth).value() == leaf_node_count);
}

TEST_CASE("invalid") {
  const auto [fen, depth, error] = GENERATE(table<std::string_view, Ply, std::string_view>({
      {STARTING_FEN, Ply{0}, "Invalid depth."},
      {STARTING_FEN, Ply{9}, "Invalid depth."},
  }));
  INFO(fen);
  const auto result = perft(parse_fen(fen).value(), depth);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() == error);
}
}  // namespace
}  // namespace prodigy::move_generator::perft
