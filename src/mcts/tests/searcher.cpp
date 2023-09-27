#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cmath>
#include <numeric>

import prodigy.core;
import prodigy.mcts;
import prodigy.move_generator;

namespace prodigy::mcts {
namespace {
TEST_CASE("search") {
  static_cast<void>(move_generator::init());
  const auto fen = GENERATE(STARTING_FEN, KIWIPETE);
  Tree tree(parse_fen(fen).value());
  const auto simulations = GENERATE(0UZ, 256UZ);
  Searcher searcher(1ULL << 30, EvaluationPolicy(), UCTPolicy(1));
  searcher.search_until(tree, [&](const auto simulation_count) { return simulation_count == simulations; });
  REQUIRE(simulations == std::accumulate(tree.root().edges().begin(), tree.root().edges().end(), 0UZ,
                                         [](const auto simulation_count, const auto& edge) {
                                           REQUIRE(std::fabsf(edge.cumulative_reward()) <= edge.simulation_count());
                                           return simulation_count + edge.simulation_count();
                                         }));
}
}  // namespace
}  // namespace prodigy::mcts
