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
      .can_en_passant = false,
  };
  static constexpr auto black_context = Node::Context{
      .side_to_move = Color::BLACK,
      .castling_rights = castling_rights,
      .can_en_passant = true,
  };
  validate.template operator()<white_context, castling_rights>();
  validate.template operator()<white_context, castling_rights & ~CastlingRights::BLACK_KINGSIDE>();
  validate.template operator()<white_context, castling_rights & ~CastlingRights::BLACK_QUEENSIDE>();
  validate.template operator()<black_context, castling_rights>();
  validate.template operator()<black_context, castling_rights & ~CastlingRights::WHITE_KINGSIDE>();
  validate.template operator()<black_context, castling_rights & ~CastlingRights::WHITE_QUEENSIDE>();
}

TEST_CASE("enable_en_passant") {
  validate([]<auto context, auto> {
    STATIC_REQUIRE(context.enable_en_passant() == Node::Context{
                                                      .side_to_move = !context.side_to_move,
                                                      .castling_rights = context.castling_rights,
                                                      .can_en_passant = true,
                                                  });
  });
}

TEST_CASE("move") {
  validate([]<auto context, auto child_castling_rights> {
    STATIC_REQUIRE(context.move(child_castling_rights) == Node::Context{
                                                              .side_to_move = !context.side_to_move,
                                                              .castling_rights = child_castling_rights,
                                                              .can_en_passant = false,
                                                          });
  });
}
}  // namespace
}  // namespace prodigy::move_generator
