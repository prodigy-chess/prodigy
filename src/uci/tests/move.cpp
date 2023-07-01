#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <type_traits>

import prodigy.core;
import prodigy.uci;

namespace prodigy::uci {
namespace {
template <auto value>
void validate() {
  STATIC_REQUIRE(to_move(value) == [] {
    Move move;
    if constexpr (std::is_same_v<decltype(value), Castle>) {
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
      .side_to_move = Color::WHITE,
      .piece_type = PieceType::PAWN,
  }>();
  validate<Capture{
      .origin = to_bitboard(Square::E4),
      .target = to_bitboard(Square::D5),
      .side_to_move = Color::WHITE,
      .aggressor = PieceType::PAWN,
      .victim = PieceType::PAWN,
  }>();
  validate<ColorTraits<Color::WHITE>::CastlingTraits<PieceType::KING>::CASTLE>();
  validate<QuietPromotion{
      .origin = to_bitboard(Square::E7),
      .target = to_bitboard(Square::E8),
      .side_to_move = Color::WHITE,
      .promotion = PieceType::QUEEN,
  }>();
  validate<CapturePromotion{
      .origin = to_bitboard(Square::E7),
      .target = to_bitboard(Square::D8),
      .side_to_move = Color::WHITE,
      .promotion = PieceType::QUEEN,
      .victim = PieceType::KNIGHT,
  }>();
  validate<EnPassantCapture{
      .origin = to_bitboard(Square::E5),
      .target = to_bitboard(Square::D6),
      .victim_origin = to_bitboard(Square::D5),
      .side_to_move = Color::WHITE,
  }>();
}
}  // namespace
}  // namespace prodigy::uci
