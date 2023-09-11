module;

#include <atomic>

module prodigy.mcts;

namespace prodigy::mcts {
SimulationCount SimulationStatistics::simulation_count() const noexcept {
  return simulation_count_.load(std::memory_order_acquire);
}

float SimulationStatistics::cumulative_reward() const noexcept {
  return cumulative_reward_.load(std::memory_order_acquire);
}

void SimulationStatistics::on_simulation_complete(const float reward) noexcept {
  // NOTE: updating statistics as a whole isn't atomic, which should only have a small effect and be tolerable.
  simulation_count_.fetch_add(1, std::memory_order_release);
  for (auto expected = cumulative_reward_.load(std::memory_order_relaxed);
       !cumulative_reward_.compare_exchange_weak(expected, expected + reward, std::memory_order_release);) {
  }
}
}  // namespace prodigy::mcts
