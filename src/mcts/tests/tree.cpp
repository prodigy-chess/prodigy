#include <catch2/catch_test_macros.hpp>

import prodigy.core;
import prodigy.mcts;
import prodigy.move_generator;

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

TEST_CASE("tree") {
  move_generator::init().value();
  {
    Tree tree(parse_fen(STARTING_FEN).value());
    // TODO: validate edges
    REQUIRE(tree.root().edges().size() == 20);
    REQUIRE_FALSE(tree.root().is_check());
  }
  {
    Tree stalemate(parse_fen("kbQ5/8/1K6/8/8/8/8/8 b - - 0 1").value());
    REQUIRE(stalemate.root().edges().empty());
    REQUIRE_FALSE(stalemate.root().is_check());
  }
  {
    Tree checkmate(parse_fen("k1Q5/8/1K6/8/8/8/8/8 b - - 0 1").value());
    REQUIRE(checkmate.root().edges().empty());
    REQUIRE(checkmate.root().is_check());
  }
}
}  // namespace
}  // namespace prodigy::mcts
