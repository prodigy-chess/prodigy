#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <optional>
#include <sstream>

import prodigy.core;
import prodigy.uci;

namespace prodigy::uci {
namespace {
template <auto value>
void validate() {
  STATIC_REQUIRE(to_move(value) == [] {
    Move move;
    if constexpr (std::same_as<decltype(value), Castle>) {
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
  validate<ColorTraits<Color::WHITE>::CastlingTraits<PieceType::KING>::CASTLE>();
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
