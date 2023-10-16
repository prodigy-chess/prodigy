#include <catch2/catch_test_macros.hpp>
#include <utility>

import prodigy.core;
import prodigy.move_generator;

namespace prodigy::move_generator {
namespace {
TEST_CASE("dispatch") {
  static constexpr auto to_node = [](const auto& position) -> decltype(auto) {
    return dispatch(position, []<auto context>(const auto& node) { return std::pair(node, context); });
  };
  {
    static constexpr auto node = to_node(STARTING_POSITION);
    STATIC_REQUIRE(node.first == Node{STARTING_POSITION.board, STARTING_POSITION.en_passant_victim_origin});
    STATIC_REQUIRE(node.second ==
                   Node::Context{
                       .side_to_move = Color::WHITE,
                       .castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE |
                                          CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE,
                       .can_en_passant = false,
                   });
  }
  {
    static constexpr auto position = parse_fen("8/8/8/8/8/8/8/8 b - e3 0 1").value();
    static constexpr auto node = to_node(position);
    STATIC_REQUIRE(node.first == Node{position.board, position.en_passant_victim_origin});
    STATIC_REQUIRE(node.second == Node::Context{
                                      .side_to_move = Color::BLACK,
                                      .castling_rights = CastlingRights(),
                                      .can_en_passant = true,
                                  });
  }
}
}  // namespace
}  // namespace prodigy::move_generator
