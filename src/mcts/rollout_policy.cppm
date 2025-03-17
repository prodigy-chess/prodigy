module;

#include <cmath>
#include <concepts>
#include <utility>

export module prodigy.mcts:rollout_policy;

import prodigy.core;
import prodigy.evaluation;
import prodigy.move_generator;

import :tree;

export namespace prodigy::mcts {
template <typename T>
concept RolloutPolicy = requires(const Position position) {
  {
    move_generator::dispatch(position, []<auto context>(const auto & node) {
      // TODO: try std::declval with T and Edge again
      T rollout_policy;
      rollout_policy.on_search_start(node.board);
      rollout_policy.on_simulation_start();
      Edge(QuietMove()).visit_move<context.side_to_move>([&](const auto& move, auto&&...) {
        rollout_policy.template on_move<context.side_to_move>(move);
      });
      return rollout_policy.template simulate<!context.side_to_move>();
    })
  } -> std::same_as<float>;
};

class EvaluationPolicy : private evaluation::Evaluator {
 public:
  using Evaluator::on_move;
  using Evaluator::on_search_start;
  using Evaluator::on_simulation_start;

  template <Color side_to_move>
  float simulate() const noexcept {
    return 2 / (1 + std::powf(10, evaluate<side_to_move>() / -400)) - 1;
  }
};
}  // namespace prodigy::mcts
