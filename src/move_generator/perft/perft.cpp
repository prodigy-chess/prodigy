module;

#include <array>
#include <cstdint>
#include <expected>
#include <map>
#include <string_view>
#include <utility>

module prodigy.move_generator.perft;

import prodigy.core;
import prodigy.move_generator;
import prodigy.uci;

namespace prodigy::move_generator::perft {
namespace {
constexpr Ply decrement(const Ply ply) noexcept { return static_cast<Ply>(std::to_underlying(ply) - 1); }

template <typename Derived, typename SubVisitor>
class Visitor : public move_generator::Visitor<Visitor<Derived, SubVisitor>> {
 public:
  constexpr explicit Visitor(Node& node, const Ply depth) noexcept : node_(node), depth_(depth) {}

  template <Node::Context context>
  constexpr void visit_pawn_move(const auto& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context>
  constexpr void visit_knight_move(const auto& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context>
  constexpr void visit_bishop_move(const auto& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context>
  constexpr void visit_rook_move(const auto& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context>
  constexpr void visit_queen_move(const auto& move) const noexcept {
    visit<context>(move);
  }

  template <Node::Context context>
  constexpr void visit_king_move(const auto& move) const noexcept {
    visit<context>(move);
  }

 private:
  template <Node::Context context>
  constexpr void visit(const auto& move) const noexcept {
    if (auto& leaf_node_count = static_cast<const Derived&>(*this).leaf_node_count(move); depth_ == Ply{0}) {
      ++leaf_node_count;
    } else {
      const auto undo = this->template scoped_move<!context.side_to_move, context.can_en_passant>(node_, move);
      walk<context>(node_, SubVisitor(node_, decrement(depth_), leaf_node_count));
    }
  }

  Node& node_;
  const Ply depth_;
};

class Perft : public Visitor<Perft, Perft> {
 public:
  constexpr explicit Perft(Node& node, const Ply depth, std::uint64_t& leaf_node_count) noexcept
      : Visitor(node, depth), leaf_node_count_(leaf_node_count) {}

  std::uint64_t& leaf_node_count(const auto&) const noexcept { return leaf_node_count_; }

 private:
  std::uint64_t& leaf_node_count_;
};

class Divide : public Visitor<Divide, Perft> {
 public:
  constexpr explicit Divide(Node& node, const Ply depth,
                            std::map<uci::Move, std::uint64_t>& move_to_leaf_node_count) noexcept
      : Visitor(node, depth), move_to_leaf_node_count_(move_to_leaf_node_count) {}

  std::uint64_t& leaf_node_count(const auto& move) const noexcept {
    return move_to_leaf_node_count_[uci::to_move(move)];
  }

 private:
  std::map<uci::Move, std::uint64_t>& move_to_leaf_node_count_;
};

template <typename Visitor, typename T>
std::expected<T, std::string_view> perft(const Position& position, const Ply depth) noexcept {
  if (depth == Ply{0}) {
    return std::unexpected("Invalid depth.");
  }
  return dispatch(position, [&]<auto context>(auto node) {
    T value{};
    walk<context>(node, Visitor(node, decrement(depth), value));
    return value;
  });
}
}  // namespace

std::expected<std::uint64_t, std::string_view> perft(const Position& position, const Ply depth) noexcept {
  return perft<Perft, std::uint64_t>(position, depth);
}

std::expected<std::map<uci::Move, std::uint64_t>, std::string_view> divide(const Position& position,
                                                                           const Ply depth) noexcept {
  return perft<Divide, std::map<uci::Move, std::uint64_t>>(position, depth);
}
}  // namespace prodigy::move_generator::perft
