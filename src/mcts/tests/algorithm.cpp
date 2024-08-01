#include <catch2/catch_test_macros.hpp>
#include <optional>

import prodigy.core;
import prodigy.mcts;
import prodigy.move_generator;

namespace prodigy::mcts {
namespace {
TEST_CASE("start") {
  static constexpr auto threads = 2UZ;
  static constexpr auto arena_bytes = 1UZ << 31;
  static constexpr auto position = parse_fen(KIWIPETE).value();
  static_cast<void>(move_generator::init());
  Algorithm algorithm(threads, arena_bytes, [] { return EvaluationPolicy(); }, [] { return UCTPolicy(2); });

  SECTION("simulations specified") {
    static constexpr auto simulations = threads * 1'000;
    REQUIRE(algorithm.start(position, simulations).has_value());
    const auto tree = algorithm.join().value();
    REQUIRE(tree != nullptr);
    REQUIRE(tree->simulation_count() == simulations);
  }

  SECTION("max simulations") {
    static constexpr auto simulations = std::numeric_limits<SimulationCount>::max();
    STATIC_REQUIRE(simulations > arena_bytes);
    REQUIRE(algorithm.start(position, simulations).has_value());
    const auto tree = algorithm.join().value();
    REQUIRE(tree != nullptr);
    REQUIRE(tree->simulation_count() < simulations);
  }

  SECTION("simulations unspecified") {
    REQUIRE(algorithm.start(position, std::nullopt).has_value());
    REQUIRE(algorithm.stop().has_value());
    REQUIRE(algorithm.join().value() != nullptr);
  }

  SECTION("poll without searching") {
    const auto result = algorithm.poll();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == "Not searching.");
  }

  SECTION("poll without stop") {
    static constexpr auto simulations = threads * 1'000;
    REQUIRE(algorithm.start(position, simulations).has_value());
    while (!algorithm.poll().value()) {
    }
    REQUIRE(algorithm.poll().value());
    const auto tree = algorithm.join().value();
    REQUIRE(tree != nullptr);
    REQUIRE(tree->simulation_count() == simulations);
  }

  SECTION("stop between poll") {
    REQUIRE(algorithm.start(position, std::nullopt).has_value());
    REQUIRE(algorithm.poll().has_value());
    REQUIRE(algorithm.stop().has_value());
    while (!algorithm.poll().value()) {
    }
    REQUIRE(algorithm.poll().value());
    REQUIRE(algorithm.join().value() != nullptr);
  }

  SECTION("stop without searching") {
    const auto result = algorithm.stop();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == "Not searching.");
  }

  SECTION("multiple stops") {
    REQUIRE(algorithm.start(position, std::nullopt).has_value());
    REQUIRE(algorithm.stop().has_value());
    REQUIRE(algorithm.stop().has_value());
    REQUIRE(algorithm.join().value() != nullptr);
  }

  SECTION("join without searching") {
    const auto result = algorithm.join();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == "Not searching.");
  }

  REQUIRE(algorithm.start(position, std::nullopt).has_value());
}
}  // namespace
}  // namespace prodigy::mcts
