module;

#include <concepts>
#include <span>

export module prodigy.mcts:tree_policy;

import :simulation_statistics;
import :tree;

export namespace prodigy::mcts {
template <typename T>
concept TreePolicy =
    requires(const T tree_policy, const SimulationCount parent_visit_count, const std::span<Edge> edges) {
      { tree_policy.select(parent_visit_count, edges) } noexcept -> std::same_as<Edge&>;
    };

class UCTPolicy {
 public:
  explicit UCTPolicy(float exploration_constant) noexcept;

  Edge& select(SimulationCount parent_visit_count, std::span<Edge>) const noexcept;

  float upper_confidence_bound(float ln_parent_visit_count, SimulationCount visit_count,
                               float cumulative_reward) const noexcept;

 private:
  float exploration_constant_;
};
}  // namespace prodigy::mcts
