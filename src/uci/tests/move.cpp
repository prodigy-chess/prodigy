#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <concepts>
#include <expected>
#include <optional>
#include <sstream>
#include <string_view>

import prodigy.core;
import prodigy.uci;

namespace prodigy::uci {
namespace {
template <auto value>
void validate() {
  STATIC_REQUIRE(to_move(value) == [] {
    Move move;
    if constexpr (std::derived_from<decltype(value), Castle>) {
      move.origin = square_of(value.king_origin);
      move.target = square_of(value.king_target);
    } else {
      move.origin = square_of(value.origin);
      move.target = square_of(value.target);
    }
    if constexpr (requires { value.promotion; }) {
      move.promotion = value.promotion;
    }
    return move;
  }());
}

TEST_CASE("to_move") {
  validate<QuietMove{
      .origin = to_bitboard(Square::E2),
      .target = to_bitboard(Square::E4),
      .piece_type = PieceType::PAWN,
  }>();
  validate<Capture{
      .origin = to_bitboard(Square::E4),
      .target = to_bitboard(Square::D5),
      .aggressor = PieceType::PAWN,
      .victim = PieceType::PAWN,
  }>();
  validate<ColorTraits<Color::WHITE>::KINGSIDE_CASTLE>();
  validate<QuietPromotion{
      .origin = to_bitboard(Square::E7),
      .target = to_bitboard(Square::E8),
      .promotion = PieceType::QUEEN,
  }>();
  validate<CapturePromotion{
      .origin = to_bitboard(Square::E7),
      .target = to_bitboard(Square::D8),
      .promotion = PieceType::QUEEN,
      .victim = PieceType::KNIGHT,
  }>();
  validate<EnPassant{
      .origin = to_bitboard(Square::E5),
      .target = to_bitboard(Square::D6),
      .victim_origin = to_bitboard(Square::D5),
  }>();
}

TEST_CASE("valid move") {
  STATIC_REQUIRE(parse_move("d2d4").value() == Move{
                                                   .origin = Square::D2,
                                                   .target = Square::D4,
                                               });
  STATIC_REQUIRE(parse_move("e7e8n").value() == Move{
                                                    .origin = Square::E7,
                                                    .target = Square::E8,
                                                    .promotion = PieceType::KNIGHT,
                                                });
  STATIC_REQUIRE(parse_move("f7f8b").value() == Move{
                                                    .origin = Square::F7,
                                                    .target = Square::F8,
                                                    .promotion = PieceType::BISHOP,
                                                });
  STATIC_REQUIRE(parse_move("g2h1r").value() == Move{
                                                    .origin = Square::G2,
                                                    .target = Square::H1,
                                                    .promotion = PieceType::ROOK,
                                                });
  STATIC_REQUIRE(parse_move("h2g1q").value() == Move{
                                                    .origin = Square::H2,
                                                    .target = Square::G1,
                                                    .promotion = PieceType::QUEEN,
                                                });
}

TEST_CASE("invalid move") {
  const auto [move, error] = GENERATE(table<std::string_view, std::string_view>({
      {"", "Too few characters."},
      {"a2a", "Too few characters."},
      {"b7b8qq", "Too many characters."},
      {"    ", "Invalid origin."},
      {"c0c4", "Invalid origin."},
      {"d2d9", "Invalid target."},
      {"e7e8x", "Invalid promotion."},
  }));
  INFO(move);
  const auto result = parse_move(move);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() == error);
}

TEST_CASE("output stream") {
  REQUIRE("e7e6" == (std::ostringstream() << to_move(QuietMove{
                         .origin = to_bitboard(Square::E7),
                         .target = to_bitboard(Square::E6),
                         .piece_type = PieceType::PAWN,
                     }))
                        .str());
  REQUIRE("d7e8q" == (std::ostringstream() << to_move(CapturePromotion{
                          .origin = to_bitboard(Square::D7),
                          .target = to_bitboard(Square::E8),
                          .promotion = PieceType::QUEEN,
                          .victim = PieceType::ROOK,
                      }))
                         .str());
}
}  // namespace
}  // namespace prodigy::uci
