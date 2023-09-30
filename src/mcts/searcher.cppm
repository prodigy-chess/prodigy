module;

#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

export module prodigy.mcts:searcher;

import prodigy.core;
import prodigy.move_generator;

import :arena;
import :expand;
import :rollout_policy;
import :simulation_statistics;
import :tree;
import :tree_policy;

export namespace prodigy::mcts {
template <RolloutPolicy RolloutPolicy, TreePolicy TreePolicy>
class alignas(64) Searcher {
 public:
  explicit Searcher(const std::size_t arena_bytes, RolloutPolicy&& rollout_policy, TreePolicy&& tree_policy)
      : arena_(arena_bytes), rollout_policy_(std::move(rollout_policy)), tree_policy_(std::move(tree_policy)) {
    path_.reserve(64);
  }

  void search_until(Tree& tree, std::invocable<SimulationCount> auto&& stop) noexcept {
    arena_.reset(arena_.size());
    rollout_policy_.on_search_start(tree.position().board);
    move_generator::dispatch(tree.position(), [&]<auto context>(const auto& node) {
      for (SimulationCount simulation_count = 0; !std::invoke(std::forward<decltype(stop)>(stop), simulation_count);
           ++simulation_count) {
        rollout_policy_.on_simulation_start();
        path_ = {tree};
        node_ = node;
        auto reward = traverse<context>(tree.root());
        for (auto it = path_.rbegin(); it != path_.rend(); ++it) {
          it->get().on_simulation_complete(reward);
          reward = -reward;
        }
      }
    });
  }

 private:
  template <move_generator::Node::Context context>
  float traverse(Node& node) noexcept {
    assert(!path_.empty());
    const auto edges = node.edges();
    if (edges.empty()) {
      return node.is_check();
    }
    Edge& edge = tree_policy_.select(path_.back().get().simulation_count(), edges);
    return edge.visit_move<context.side_to_move>(visitor{
        [&](const auto& move, Edge::EnableEnPassant) { return traverse<context.enable_en_passant()>(edge, move); },
        [&](const auto& move, const CastlingRights child_castling_rights) {
          return dispatch(child_castling_rights, [&]<auto child_castling_rights> {
            return traverse<context.move(child_castling_rights)>(edge, move);
          });
        },
        [&](const Castle& move) {
          return traverse<context.move(context.castling_rights & ~ColorTraits<context.side_to_move>::CASTLING_RIGHTS)>(
              edge, move);
        },
        [&](const auto& move) { return traverse<context.move(context.castling_rights)>(edge, move); },
    });
  }

  template <move_generator::Node::Context child_context>
  float traverse(Edge& edge, const auto& move) noexcept {
    path_.emplace_back(edge);
    node_.board.apply<!child_context.side_to_move>(move);
    if constexpr (child_context.can_en_passant) {
      node_.en_passant_victim_origin = move.target;
    }
    rollout_policy_.template on_move<!child_context.side_to_move>(move);
    const auto [child, created] =
        edge.get_or_create_child([&] -> decltype(auto) { return expand<child_context>(node_, arena_); },
                                 [&](const auto& node) { arena_.reset(sizeof(node) + node.edges().size_bytes()); });
    return created ? rollout_policy_.template simulate<!child_context.side_to_move>() : traverse<child_context>(child);
  }

  Arena arena_;
  RolloutPolicy rollout_policy_;
  const TreePolicy tree_policy_;
  std::vector<std::reference_wrapper<SimulationStatistics>> path_;
  move_generator::Node node_;
};

template <RolloutPolicy RolloutPolicy, TreePolicy TreePolicy>
explicit Searcher(std::size_t, RolloutPolicy, TreePolicy) -> Searcher<RolloutPolicy, TreePolicy>;
}  // namespace prodigy::mcts
