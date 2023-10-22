module;

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <expected>
#include <functional>
#include <future>
#include <limits>
#include <memory>
#include <optional>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

export module prodigy.mcts:algorithm;

import prodigy.core;

import :arena;
import :rollout_policy;
import :searcher;
import :simulation_statistics;
import :tree;
import :tree_policy;

export namespace prodigy::mcts {
template <RolloutPolicy RolloutPolicy, TreePolicy TreePolicy>
class Algorithm {
 public:
  explicit Algorithm(const std::optional<std::size_t> threads, const std::size_t arena_bytes,
                     const std::function<RolloutPolicy()>& make_rollout_policy,
                     const std::function<TreePolicy()>& make_tree_policy)
      : searchers_([&] {
          decltype(searchers_) searchers;
          searchers.reserve(std::max(threads.value_or(std::thread::hardware_concurrency()), 1UZ));
          const auto arena_bytes_per_searcher =
              arena_bytes / searchers.capacity() / Arena::ALIGNMENT * Arena::ALIGNMENT;
          for (auto i = 0UZ; i < searchers.capacity(); ++i) {
            searchers.emplace_back(arena_bytes_per_searcher, make_rollout_policy(), make_tree_policy());
          }
          return searchers;
        }()),
        max_simulations_per_searcher_(
            std::min<std::common_type_t<std::size_t, SimulationCount>>(arena_bytes / (sizeof(Node) + sizeof(Edge) * 45),
                                                                       std::numeric_limits<SimulationCount>::max()) /
            searchers_.size()) {}

  Algorithm(const Algorithm&) = delete;
  Algorithm& operator=(const Algorithm&) = delete;

  Algorithm(Algorithm&&) = delete;
  Algorithm& operator=(Algorithm&&) = delete;

  ~Algorithm() {
    static_cast<void>(stop());
    static_cast<void>(join());
  }

  [[nodiscard]] std::expected<void, std::string_view> start(const Position& position,
                                                            const std::optional<std::size_t> simulations) noexcept {
    if (search_state_.has_value()) {
      return std::unexpected("Already searching.");
    }
    auto search_tree = std::make_unique<Tree>(position);
    auto& [stop, tree, searches] = search_state_.emplace();
    for (const auto simulations_per_searcher = simulations
                                                   .transform([&](const auto simulations) {
                                                     return std::min<std::common_type_t<std::size_t, SimulationCount>>(
                                                         simulations / searchers_.size(),
                                                         max_simulations_per_searcher_);
                                                   })
                                                   .value_or(max_simulations_per_searcher_);
         auto& searcher : searchers_) {
      searches.push_back(std::async(std::launch::async, [&stop = std::as_const(stop), &search_tree = *search_tree,
                                                         &searcher, simulations_per_searcher] {
        searcher.search_until(search_tree, [&](const auto simulation_count) {
          return stop.test(std::memory_order_relaxed) || simulation_count == simulations_per_searcher;
        });
      }));
    }
    tree = std::move(search_tree);
    return {};
  }

  [[nodiscard]] std::expected<bool, std::string_view> poll() const noexcept {
    if (!search_state_.has_value()) {
      return std::unexpected("Not searching.");
    }
    return std::ranges::all_of(search_state_->searches, [](const auto& search) {
      return search.wait_for(std::chrono::steady_clock::duration::zero()) == std::future_status::ready;
    });
  }

  [[nodiscard]] std::expected<void, std::string_view> stop() noexcept {
    if (!search_state_.has_value()) {
      return std::unexpected("Not searching.");
    }
    search_state_->stop.test_and_set(std::memory_order_relaxed);
    return {};
  }

  [[nodiscard]] std::expected<std::unique_ptr<const Tree>, std::string_view> join() noexcept {
    if (!search_state_.has_value()) {
      return std::unexpected("Not searching.");
    }
    auto tree = std::move(search_state_->tree);
    search_state_.reset();
    return tree;
  }

 private:
  struct SearchState {
    std::atomic_flag stop;
    std::unique_ptr<const Tree> tree;
    std::vector<std::future<void>> searches;
  };

  std::optional<SearchState> search_state_;
  std::vector<Searcher<RolloutPolicy, TreePolicy>> searchers_;
  const SimulationCount max_simulations_per_searcher_;
};

template <typename MakeRolloutPolicy, typename MakeTreePolicy>
explicit Algorithm(std::size_t, std::size_t, MakeRolloutPolicy&&, MakeTreePolicy&&)
    -> Algorithm<std::invoke_result_t<MakeRolloutPolicy>, std::invoke_result_t<MakeTreePolicy>>;
}  // namespace prodigy::mcts
