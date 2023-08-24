#include <catch2/catch_test_macros.hpp>

import prodigy.mcts;

namespace prodigy::mcts {
namespace {
TEST_CASE("node") {
  Arena arena(sizeof(Node) * 64);
  // TODO: add tests for nodes with edges
  {
    auto& stalemate = arena.new_object<Node>(0, false);
    REQUIRE(stalemate.edges().empty());
    REQUIRE_FALSE(stalemate.is_check());
  }
  {
    auto& checkmate = arena.new_object<Node>(0, true);
    REQUIRE(checkmate.edges().empty());
    REQUIRE(checkmate.is_check());
  }
}
}  // namespace
}  // namespace prodigy::mcts
