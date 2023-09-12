#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>

import prodigy.core;
import prodigy.mcts;

namespace prodigy::mcts {
namespace {
TEST_CASE("UCT upper_confidence_bound") {
  using namespace Catch::Matchers;
  const UCTPolicy tree_policy(std::sqrtf(2));
  REQUIRE(std::isnan(tree_policy.upper_confidence_bound(std::logf(1), 0, 0)));
  REQUIRE_THAT(tree_policy.upper_confidence_bound(std::logf(2), 1, 0.999), WithinAbs(2.17641f, 0.000001));
  REQUIRE_THAT(tree_policy.upper_confidence_bound(std::logf(56'789), 4'321, 2'851.86), WithinAbs(0.731182f, 0.000001));
  REQUIRE_THAT(tree_policy.upper_confidence_bound(std::logf(123'456), 789, -752.706), WithinAbs(-0.781612f, 0.000001));
}

TEST_CASE("UCT select") {
  const UCTPolicy tree_policy(std::sqrtf(2));
  std::array<Edge, 3> edges{
      Edge(QuietMove{
          .origin = to_bitboard(Square::E2),
          .target = to_bitboard(Square::E3),
          .piece_type = PieceType::PAWN,
      }),
      Edge(QuietMove{
          .origin = to_bitboard(Square::F2),
          .target = to_bitboard(Square::F3),
          .piece_type = PieceType::PAWN,
      }),
      Edge(QuietMove{
          .origin = to_bitboard(Square::G2),
          .target = to_bitboard(Square::G3),
          .piece_type = PieceType::PAWN,
      }),
  };
  auto select = [&, parent_visit_count = 0] mutable -> decltype(auto) {
    return tree_policy.select(++parent_visit_count, edges);
  };
  {
    auto& edge = select();
    REQUIRE(&edge == &edges[0]);
    edge.on_simulation_complete(0.1);
  }
  {
    auto& edge = select();
    REQUIRE(&edge == &edges[1]);
    edge.on_simulation_complete(0.2);
  }
  {
    auto& edge = select();
    REQUIRE(&edge == &edges[2]);
    edge.on_simulation_complete(0.05);
  }
  {
    auto& edge = select();
    REQUIRE(&edge == &edges[1]);
    edge.on_simulation_complete(-0.88);
  }
  {
    auto& edge = select();
    REQUIRE(&edge == &edges[0]);
    edge.on_simulation_complete(0.293);
  }
  REQUIRE(&select() == &edges[2]);
}
}  // namespace
}  // namespace prodigy::mcts
