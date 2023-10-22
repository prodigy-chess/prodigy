#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <optional>

import prodigy.core;
import prodigy.engine;
import prodigy.mcts;
import prodigy.move_generator;

namespace prodigy {
namespace {
TEST_CASE("strategy") {
  static_cast<void>(move_generator::init());
  MCTSStrategy<mcts::EvaluationPolicy, mcts::UCTPolicy> strategy(
      2UZ, 1UZ << 31, [] { return mcts::EvaluationPolicy(); }, [] { return mcts::UCTPolicy(2); });
  SECTION("start") {
    REQUIRE_NOTHROW(strategy.start(STARTING_POSITION, std::nullopt));
    REQUIRE_THROWS(strategy.start(STARTING_POSITION, std::nullopt));
  }
  SECTION("poll") {
    REQUIRE_THROWS(strategy.poll());
    REQUIRE_NOTHROW(strategy.start(STARTING_POSITION, 100));
    while (!strategy.poll()) {
    }
    REQUIRE(strategy.poll());
  }
  SECTION("stop") {
    REQUIRE_THROWS(strategy.stop());
    REQUIRE_NOTHROW(strategy.start(STARTING_POSITION, 100));
    REQUIRE_NOTHROW(strategy.stop());
    REQUIRE_NOTHROW(strategy.stop());
  }
  SECTION("join") {
    REQUIRE_THROWS(strategy.join());
    REQUIRE_NOTHROW(strategy.start(STARTING_POSITION, 100));
    REQUIRE(strategy.join().has_value());
    REQUIRE_THROWS(strategy.join());
  }
  SECTION("checkmate") {
    REQUIRE_NOTHROW(strategy.start(parse_fen("3k3R/R7/8/8/8/8/8/4K3 b - - 0 1").value(), std::nullopt));
    REQUIRE_NOTHROW(strategy.stop());
    REQUIRE_FALSE(strategy.join().has_value());
  }
}
}  // namespace
}  // namespace prodigy
