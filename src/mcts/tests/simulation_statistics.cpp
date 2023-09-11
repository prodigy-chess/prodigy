#include <catch2/catch_test_macros.hpp>

import prodigy.mcts;

namespace prodigy::mcts {
namespace {
TEST_CASE("on_simulation_complete") {
  SimulationStatistics statistics;
  REQUIRE(statistics.simulation_count() == 0);
  REQUIRE(statistics.cumulative_reward() == 0);
  statistics.on_simulation_complete(0);
  REQUIRE(statistics.simulation_count() == 1);
  REQUIRE(statistics.cumulative_reward() == 0);
  statistics.on_simulation_complete(1);
  REQUIRE(statistics.simulation_count() == 2);
  REQUIRE(statistics.cumulative_reward() == 1);
  statistics.on_simulation_complete(0.234567);
  REQUIRE(statistics.simulation_count() == 3);
  REQUIRE(statistics.cumulative_reward() == 1.234567f);
}
}  // namespace
}  // namespace prodigy::mcts
