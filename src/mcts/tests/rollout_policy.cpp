#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

import prodigy.core;
import prodigy.mcts;

namespace prodigy::mcts {
namespace {
template <Color side_to_move>
void simulate(const EvaluationPolicy& rollout_policy, const float expected_reward) noexcept {
  REQUIRE_THAT(rollout_policy.simulate<side_to_move>(), Catch::Matchers::WithinAbs(expected_reward, 0.00001));
  REQUIRE_THAT(rollout_policy.simulate<!side_to_move>(), Catch::Matchers::WithinAbs(-expected_reward, 0.00001));
}

TEST_CASE("evaluation policy") {
  EvaluationPolicy rollout_policy;
  rollout_policy.on_search_start(STARTING_POSITION.board);
  rollout_policy.on_simulation_start();
  simulate<Color::WHITE>(rollout_policy, 0);
  rollout_policy.on_move<Color::WHITE>(QuietMove{
      .origin = to_bitboard(Square::E2),
      .target = to_bitboard(Square::E4),
      .piece_type = PieceType::PAWN,
  });
  simulate<Color::BLACK>(rollout_policy, -0.09184);
  rollout_policy.on_move<Color::BLACK>(QuietMove{
      .origin = to_bitboard(Square::E6),
      .target = to_bitboard(Square::E5),
      .piece_type = PieceType::PAWN,
  });
  simulate<Color::WHITE>(rollout_policy, 0.05176);
  rollout_policy.on_move<Color::WHITE>(QuietMove{
      .origin = to_bitboard(Square::F1),
      .target = to_bitboard(Square::C4),
      .piece_type = PieceType::BISHOP,
  });
  simulate<Color::BLACK>(rollout_policy, -0.12314);
  rollout_policy.on_move<Color::BLACK>(QuietMove{
      .origin = to_bitboard(Square::D8),
      .target = to_bitboard(Square::G5),
      .piece_type = PieceType::QUEEN,
  });
  simulate<Color::WHITE>(rollout_policy, 0.14293);
  rollout_policy.on_move<Color::WHITE>(QuietMove{
      .origin = to_bitboard(Square::D1),
      .target = to_bitboard(Square::H5),
      .piece_type = PieceType::QUEEN,
  });
  simulate<Color::BLACK>(rollout_policy, -0.11746);
  rollout_policy.on_move<Color::BLACK>(QuietMove{
      .origin = to_bitboard(Square::G5),
      .target = to_bitboard(Square::F5),
      .piece_type = PieceType::QUEEN,
  });
  simulate<Color::WHITE>(rollout_policy, 0.13728);
  rollout_policy.on_move<Color::WHITE>(Capture{
      .origin = to_bitboard(Square::H5),
      .target = to_bitboard(Square::F5),
      .aggressor = PieceType::QUEEN,
      .victim = PieceType::QUEEN,
  });
  simulate<Color::BLACK>(rollout_policy, -0.99577);
}
}  // namespace
}  // namespace prodigy::mcts
