module;

#include <cassert>
#include <cmath>
#include <limits>
#include <span>

module prodigy.mcts;

namespace prodigy::mcts {
UCTPolicy::UCTPolicy(const float exploration_constant) noexcept : exploration_constant_(exploration_constant) {}

Edge& UCTPolicy::select(const SimulationCount parent_visit_count, const std::span<Edge> edges) const noexcept {
  assert(parent_visit_count >= 1);
  assert(!edges.empty());
  const auto ln_parent_visit_count = std::logf(parent_visit_count);
  Edge* choice;
  for (auto max_ucb = std::numeric_limits<float>::lowest(); auto& edge : edges) {
    const auto visit_count = edge.simulation_count();
    if (visit_count == 0) {
      return edge;
    }
    if (const auto ucb = upper_confidence_bound(ln_parent_visit_count, visit_count, edge.cumulative_reward());
        ucb > max_ucb) {
      max_ucb = ucb;
      choice = &edge;
    }
  }
  return *choice;
}

float UCTPolicy::upper_confidence_bound(const float ln_parent_visit_count, const SimulationCount visit_count,
                                        const float cumulative_reward) const noexcept {
  assert(!std::signbit(ln_parent_visit_count));
  return cumulative_reward / visit_count + exploration_constant_ * std::sqrtf(ln_parent_visit_count / visit_count);
}
}  // namespace prodigy::mcts
