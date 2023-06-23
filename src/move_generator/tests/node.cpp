#include <catch2/catch_test_macros.hpp>
#include <magic_enum_utility.hpp>
#include <utility>

import prodigy.core;
import prodigy.move_generator;

namespace prodigy::move_generator {
namespace {
TEST_CASE("move") {
  static constexpr auto validate = [](auto&& validate) {
    std::forward<decltype(validate)>(validate)
        .template operator()<Node::Context{
            .side_to_move = Color::WHITE,
            .castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE |
                               CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE,
            .has_en_passant_target = false,
        }>();
    std::forward<decltype(validate)>(validate)
        .template operator()<Node::Context{
            .side_to_move = Color::BLACK,
            .castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE |
                               CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE,
            .has_en_passant_target = true,
        }>();
  };
  magic_enum::enum_for_each<Node::Context::Move>([](const auto move) {
    switch (move) {
      case Node::Context::Move::PAWN_DOUBLE_PUSH:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(Node::Context::Move::PAWN_DOUBLE_PUSH) ==
                         Node::Context{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.castling_rights,
                             .has_en_passant_target = true,
                         });
        });
        break;
      case Node::Context::Move::KINGSIDE_ROOK_MOVE:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(Node::Context::Move::KINGSIDE_ROOK_MOVE) ==
                         Node::Context{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.side_to_move == Color::WHITE
                                                    ? node_context.castling_rights & ~CastlingRights::WHITE_KINGSIDE
                                                    : node_context.castling_rights & ~CastlingRights::BLACK_KINGSIDE,
                             .has_en_passant_target = false,
                         });
        });
        break;
      case Node::Context::Move::QUEENSIDE_ROOK_MOVE:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(Node::Context::Move::QUEENSIDE_ROOK_MOVE) ==
                         Node::Context{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.side_to_move == Color::WHITE
                                                    ? node_context.castling_rights & ~CastlingRights::WHITE_QUEENSIDE
                                                    : node_context.castling_rights & ~CastlingRights::BLACK_QUEENSIDE,
                             .has_en_passant_target = false,
                         });
        });
        break;
      case Node::Context::Move::KING_MOVE:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(Node::Context::Move::KING_MOVE) ==
                         Node::Context{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.side_to_move == Color::WHITE
                                                    ? node_context.castling_rights & ~(CastlingRights::WHITE_KINGSIDE |
                                                                                       CastlingRights::WHITE_QUEENSIDE)
                                                    : node_context.castling_rights & ~(CastlingRights::BLACK_KINGSIDE |
                                                                                       CastlingRights::BLACK_QUEENSIDE),
                             .has_en_passant_target = false,
                         });
        });
        break;
      case Node::Context::Move::OTHER:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(Node::Context::Move::OTHER) ==
                         Node::Context{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.castling_rights,
                             .has_en_passant_target = false,
                         });
          STATIC_REQUIRE(node_context.move(Node::Context::Move::OTHER) == node_context.move());
        });
        break;
    }
  });
}
}  // namespace
}  // namespace prodigy::move_generator
