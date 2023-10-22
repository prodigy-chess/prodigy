module;

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <utility>

export module prodigy.engine:mcts_strategy;

import prodigy.core;
import prodigy.mcts;
import prodigy.uci;

import :strategy;

export namespace prodigy {
template <mcts::RolloutPolicy RolloutPolicy, mcts::TreePolicy TreePolicy>
class MCTSStrategy final : public Strategy {
 public:
  template <typename... Args>
  explicit MCTSStrategy(Args&&... args) : algorithm_(std::forward<Args>(args)...) {}

  void start(const Position& position, const std::optional<std::size_t> nodes) override {
    algorithm_.start(position, nodes).value();
  }

  [[nodiscard]] bool poll() override { return algorithm_.poll().value(); }

  void stop() override { algorithm_.stop().value(); }

  [[nodiscard]] std::optional<uci::Move> join() override {
    if (const auto tree = algorithm_.join().value(); !tree->root().edges().empty()) {
      const auto select_move = [&]<auto side_to_move> {
        return std::ranges::max_element(tree->root().edges(), std::less(), &mcts::Edge::simulation_count)
            ->template visit_move<side_to_move>([](const auto& move, auto&&...) { return uci::to_move(move); });
      };
      switch (tree->position().side_to_move) {
        case Color::WHITE:
          return select_move.template operator()<Color::WHITE>();
        case Color::BLACK:
          return select_move.template operator()<Color::BLACK>();
      }
    }
    return std::nullopt;
  }

 private:
  mcts::Algorithm<RolloutPolicy, TreePolicy> algorithm_;
};
}  // namespace prodigy
