#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

import prodigy.core;
import prodigy.mcts;
import prodigy.move_generator;
import prodigy.uci;

namespace prodigy::mcts {
namespace {
TEST_CASE("search") {
  const auto [fen, expected_search_results] = GENERATE(table<std::string_view,
                                                             std::map<std::string, std::pair<SimulationCount, float>>>({
      {
          STARTING_FEN,
          {
              {"a2a3", {791, 17.1234}},  {"a2a4", {784, 15.2436}},  {"b1a3", {739, 3.88427}},
              {"b1c3", {1072, 94.1726}}, {"b2b3", {736, 3.18043}},  {"b2b4", {756, 8.15718}},
              {"c2c3", {832, 27.647}},   {"c2c4", {850, 32.3108}},  {"d2d3", {867, 36.978}},
              {"d2d4", {1098, 101.819}}, {"e2e3", {916, 50.207}},   {"e2e4", {1072, 94.1889}},
              {"f2f3", {591, -31.0122}}, {"f2f4", {626, -23.1671}}, {"g1f3", {1100, 102.171}},
              {"g1h3", {759, 8.77295}},  {"g2g3", {731, 1.90581}},  {"g2g4", {584, -32.4011}},
              {"h2h3", {789, 16.5869}},  {"h2h4", {691, -8.10659}},
          },
      },
      {
          KIWIPETE,
          {
              {"a1b1", {298, 64.4146}}, {"a1c1", {318, 75.8975}},  {"a1d1", {338, 88.5245}}, {"a2a3", {303, 67.7185}},
              {"a2a4", {300, 65.5204}}, {"b2b3", {283, 55.1439}},  {"c3a4", {260, 42.2498}}, {"c3b1", {262, 43.3837}},
              {"c3b5", {300, 64.704}},  {"c3d1", {247, 34.7072}},  {"d2c1", {273, 49.3455}}, {"d2e3", {312, 72.9894}},
              {"d2f4", {250, 36.714}},  {"d2g5", {295, 62.5639}},  {"d2h6", {255, 39.044}},  {"d5d6", {302, 66.5816}},
              {"d5e6", {576, 248.926}}, {"e1c1", {250, 36.6904}},  {"e1d1", {228, 24.1192}}, {"e1f1", {257, 40.4664}},
              {"e1g1", {261, 42.2124}}, {"e2a6", {2329, 1650.53}}, {"e2b5", {272, 49.3122}}, {"e2c4", {270, 47.9487}},
              {"e2d1", {274, 50.2139}}, {"e2d3", {294, 62.2941}},  {"e2f1", {273, 49.6308}}, {"e5c4", {249, 35.8396}},
              {"e5c6", {265, 45.2073}}, {"e5d3", {259, 41.4986}},  {"e5d7", {306, 69.3977}}, {"e5f7", {475, 177.606}},
              {"e5g4", {259, 41.8291}}, {"e5g6", {565, 241.031}},  {"f3d3", {222, 21.1434}}, {"f3e3", {279, 53.3381}},
              {"f3f4", {276, 51.4142}}, {"f3f5", {242, 31.6412}},  {"f3f6", {589, 256.913}}, {"f3g3", {261, 42.9996}},
              {"f3g4", {228, 23.9852}}, {"f3h3", {352, 97.7664}},  {"f3h5", {240, 30.5593}}, {"g2g3", {271, 48.3868}},
              {"g2g4", {257, 40.5931}}, {"g2h3", {297, 64.0295}},  {"h1f1", {336, 86.8992}}, {"h1g1", {276, 51.2164}},
          },
      },
      {
          "3rkr2/8/8/8/8/8/q7/4K3 w - - 0 1",
          {},
      },
      {
          "4k3/8/8/8/8/8/7r/q3K3 w - - 0 1",
          {},
      },
  }));
  static constexpr SimulationCount simulations = 1 << 14;
  static_cast<void>(move_generator::init());
  Searcher searcher(1 << 30, EvaluationPolicy(), UCTPolicy(3 * std::sqrtf(2)));
  const auto position = parse_fen(fen).value();
  Tree tree(position);
  REQUIRE(tree.root().edges().size() == expected_search_results.size());
  searcher.search_until(tree, [](const auto simulation_count) { return simulation_count == simulations; });
  REQUIRE(tree.simulation_count() == simulations);
  const auto search_results = move_generator::dispatch(position, [&]<auto context>(auto&&...) {
    std::map<std::string, std::pair<SimulationCount, float>> search_results;
    for (const auto& edge : tree.root().edges()) {
      edge.visit_move<context.side_to_move>([&](const auto& move, auto&&...) {
        REQUIRE(search_results
                    .try_emplace((std::ostringstream() << uci::to_move(move)).str(), edge.simulation_count(),
                                 edge.cumulative_reward())
                    .second);
      });
    }
    return search_results;
  });
  for (const auto& [move, statistics] : search_results) {
    INFO(move);
    const auto it = expected_search_results.find(move);
    REQUIRE(it != expected_search_results.end());
    const auto& [expected_simulation_count, expected_cumulative_reward] = it->second;
    CHECK(statistics.first == expected_simulation_count);
    CHECK_THAT(statistics.second, Catch::Matchers::WithinRel(expected_cumulative_reward, 0.001f));
  }
}
}  // namespace
}  // namespace prodigy::mcts
