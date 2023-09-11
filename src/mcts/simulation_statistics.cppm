module;

#include <atomic>
#include <cstdint>

export module prodigy.mcts:simulation_statistics;

export namespace prodigy::mcts {
using SimulationCount = std::uint32_t;

class SimulationStatistics {
 public:
  SimulationCount simulation_count() const noexcept;

  float cumulative_reward() const noexcept;

  void on_simulation_complete(float reward) noexcept;

 private:
  std::atomic<SimulationCount> simulation_count_ = 0;
  std::atomic<float> cumulative_reward_ = 0;
};
}  // namespace prodigy::mcts
