#include <catch2/catch_test_macros.hpp>

import prodigy.core;
import prodigy.move_generator;

namespace prodigy::move_generator {
namespace {
void validate(const auto& validate) {
  static constexpr auto castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE |
                                          CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE;
  static constexpr auto white_context = Node::Context{
      .side_to_move = Color::WHITE,
      .castling_rights = castling_rights,
      .has_en_passant_target = false,
  };
  static constexpr auto black_context = Node::Context{
      .side_to_move = Color::BLACK,
      .castling_rights = castling_rights,
      .has_en_passant_target = true,
  };
  validate.template operator()<white_context, castling_rights>();
  validate.template operator()<white_context, castling_rights & ~CastlingRights::BLACK_KINGSIDE>();
  validate.template operator()<white_context, castling_rights & ~CastlingRights::BLACK_QUEENSIDE>();
  validate.template operator()<black_context, castling_rights>();
  validate.template operator()<black_context, castling_rights & ~CastlingRights::WHITE_KINGSIDE>();
  validate.template operator()<black_context, castling_rights & ~CastlingRights::WHITE_QUEENSIDE>();
}

TEST_CASE("pawn_double_push") {
  validate([]<auto context, auto> {
    STATIC_REQUIRE(context.pawn_double_push() == Node::Context{
                                                     .side_to_move = !context.side_to_move,
                                                     .castling_rights = context.castling_rights,
                                                     .has_en_passant_target = true,
                                                 });
  });
}

TEST_CASE("kingside_rook_move") {
  validate([]<auto context, auto castling_rights> {
    STATIC_REQUIRE(context.kingside_rook_move(castling_rights) ==
                   Node::Context{
                       .side_to_move = !context.side_to_move,
                       .castling_rights = context.side_to_move == Color::WHITE
                                              ? castling_rights & ~CastlingRights::WHITE_KINGSIDE
                                              : castling_rights & ~CastlingRights::BLACK_KINGSIDE,
                       .has_en_passant_target = false,
                   });
  });
}

TEST_CASE("queenside_rook_move") {
  validate([]<auto context, auto castling_rights> {
    STATIC_REQUIRE(context.queenside_rook_move(castling_rights) ==
                   Node::Context{
                       .side_to_move = !context.side_to_move,
                       .castling_rights = context.side_to_move == Color::WHITE
                                              ? castling_rights & ~CastlingRights::WHITE_QUEENSIDE
                                              : castling_rights & ~CastlingRights::BLACK_QUEENSIDE,
                       .has_en_passant_target = false,
                   });
  });
}

TEST_CASE("king_move") {
  validate([]<auto context, auto castling_rights> {
    STATIC_REQUIRE(context.king_move(castling_rights) ==
                   Node::Context{
                       .side_to_move = !context.side_to_move,
                       .castling_rights =
                           context.side_to_move == Color::WHITE
                               ? castling_rights & ~(CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE)
                               : castling_rights & ~(CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE),
                       .has_en_passant_target = false,
                   });
  });
}

TEST_CASE("move") {
  validate([]<auto context, auto castling_rights> {
    STATIC_REQUIRE(context.move(castling_rights) == Node::Context{
                                                        .side_to_move = !context.side_to_move,
                                                        .castling_rights = castling_rights,
                                                        .has_en_passant_target = false,
                                                    });
  });
}
}  // namespace
}  // namespace prodigy::move_generator
