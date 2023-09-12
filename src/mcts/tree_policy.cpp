module;

#include <cassert>
#include <cmath>
#include <functional>
#include <limits>
#include <span>

module prodigy.mcts;

namespace prodigy::mcts {
UCTPolicy::UCTPolicy(const float exploration_constant) noexcept : exploration_constant_(exploration_constant) {}

Edge& UCTPolicy::select(const SimulationCount parent_visit_count, const std::span<Edge> edges) const noexcept {
  assert(parent_visit_count != 0);
  const auto ln_parent_visit_count = std::logf(parent_visit_count);
  auto choice = std::ref(edges.front());
  static_assert(std::numeric_limits<float>::is_iec559);
  for (auto max_ucb = -std::numeric_limits<float>::infinity(); auto& edge : edges) {
    const auto visit_count = edge.simulation_count();
    if (visit_count == 0) {
      return edge;
    }
    if (const auto ucb = upper_confidence_bound(ln_parent_visit_count, visit_count, edge.cumulative_reward());
        max_ucb < ucb) {
      max_ucb = ucb;
      choice = edge;
    }
  }
  return choice;
}

float UCTPolicy::upper_confidence_bound(const float ln_parent_visit_count, const SimulationCount visit_count,
                                        const float cumulative_reward) const noexcept {
  assert(!std::signbit(ln_parent_visit_count));
  return cumulative_reward / visit_count + exploration_constant_ * std::sqrt(ln_parent_visit_count / visit_count);
}
}  // namespace prodigy::mcts
