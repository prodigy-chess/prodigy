#include <catch2/catch_test_macros.hpp>
#include <magic_enum.hpp>
#include <utility>

import prodigy.core;
import prodigy.move_generator;

namespace prodigy::move_generator {
namespace {
TEST_CASE("move") {
  static constexpr auto validate = [](auto&& validate) {
    std::forward<decltype(validate)>(validate)
        .template operator()<NodeContext{
            .side_to_move = Color::WHITE,
            .castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE |
                               CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE,
            .has_en_passant_target = false,
        }>();
    std::forward<decltype(validate)>(validate)
        .template operator()<NodeContext{
            .side_to_move = Color::BLACK,
            .castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE |
                               CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE,
            .has_en_passant_target = true,
        }>();
  };
  magic_enum::enum_for_each<NodeContext::Move>([](const auto move) {
    switch (move) {
      case NodeContext::Move::PAWN_DOUBLE_PUSH:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(NodeContext::Move::PAWN_DOUBLE_PUSH) ==
                         NodeContext{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.castling_rights,
                             .has_en_passant_target = true,
                         });
        });
        break;
      case NodeContext::Move::KINGSIDE_ROOK_MOVE:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(NodeContext::Move::KINGSIDE_ROOK_MOVE) ==
                         NodeContext{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.side_to_move == Color::WHITE
                                                    ? node_context.castling_rights & ~CastlingRights::WHITE_KINGSIDE
                                                    : node_context.castling_rights & ~CastlingRights::BLACK_KINGSIDE,
                             .has_en_passant_target = false,
                         });
        });
        break;
      case NodeContext::Move::QUEENSIDE_ROOK_MOVE:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(NodeContext::Move::QUEENSIDE_ROOK_MOVE) ==
                         NodeContext{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.side_to_move == Color::WHITE
                                                    ? node_context.castling_rights & ~CastlingRights::WHITE_QUEENSIDE
                                                    : node_context.castling_rights & ~CastlingRights::BLACK_QUEENSIDE,
                             .has_en_passant_target = false,
                         });
        });
        break;
      case NodeContext::Move::KING_MOVE:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(NodeContext::Move::KING_MOVE) ==
                         NodeContext{
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
      case NodeContext::Move::OTHER:
        validate([]<auto node_context> {
          STATIC_REQUIRE(node_context.move(NodeContext::Move::OTHER) ==
                         NodeContext{
                             .side_to_move = !node_context.side_to_move,
                             .castling_rights = node_context.castling_rights,
                             .has_en_passant_target = false,
                         });
          STATIC_REQUIRE(node_context.move(NodeContext::Move::OTHER) == node_context.move());
        });
        break;
    }
  });
}
}  // namespace
}  // namespace prodigy::move_generator
