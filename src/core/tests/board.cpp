#include <catch2/catch_test_macros.hpp>
#include <magic_enum.hpp>
#include <optional>
#include <utility>

import prodigy.core;

namespace prodigy {
namespace {
using namespace magic_enum;

TEST_CASE("empty") {
  static constexpr Board board;
  enum_for_each<Color>([](const auto color) {
    enum_for_each<PieceType>([&](const auto piece_type) { STATIC_REQUIRE(empty(board.get(color, piece_type))); });
    STATIC_REQUIRE(empty(board.all_pieces(color)));
  });
  STATIC_REQUIRE(empty(board.occupancy()));
}

TEST_CASE("starting board") {
  static constexpr auto piece_placement = [&] {
    EnumMap<Square, std::optional<std::pair<Color, PieceType>>> piece_placement;
    piece_placement[Square::A1].emplace(Color::WHITE, PieceType::ROOK);
    piece_placement[Square::B1].emplace(Color::WHITE, PieceType::KNIGHT);
    piece_placement[Square::C1].emplace(Color::WHITE, PieceType::BISHOP);
    piece_placement[Square::D1].emplace(Color::WHITE, PieceType::QUEEN);
    piece_placement[Square::E1].emplace(Color::WHITE, PieceType::KING);
    piece_placement[Square::F1].emplace(Color::WHITE, PieceType::BISHOP);
    piece_placement[Square::G1].emplace(Color::WHITE, PieceType::KNIGHT);
    piece_placement[Square::H1].emplace(Color::WHITE, PieceType::ROOK);
    piece_placement[Square::A2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::B2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::C2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::D2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::E2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::F2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::G2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::H2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::A7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::B7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::C7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::D7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::E7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::F7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::G7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::H7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::A8].emplace(Color::BLACK, PieceType::ROOK);
    piece_placement[Square::B8].emplace(Color::BLACK, PieceType::KNIGHT);
    piece_placement[Square::C8].emplace(Color::BLACK, PieceType::BISHOP);
    piece_placement[Square::D8].emplace(Color::BLACK, PieceType::QUEEN);
    piece_placement[Square::E8].emplace(Color::BLACK, PieceType::KING);
    piece_placement[Square::F8].emplace(Color::BLACK, PieceType::BISHOP);
    piece_placement[Square::G8].emplace(Color::BLACK, PieceType::KNIGHT);
    piece_placement[Square::H8].emplace(Color::BLACK, PieceType::ROOK);
    return piece_placement;
  }();
  static constexpr auto piece_to_bitboard = [&] {
    EnumMap<Color, EnumMap<PieceType, Bitboard>> piece_to_bitboard{};
    enum_for_each<Square>([&](const auto square) {
      if (const auto piece = piece_placement[square]; piece.has_value()) {
        piece_to_bitboard[piece->first][piece->second] |= to_bitboard(square);
      }
    });
    return piece_to_bitboard;
  }();
  static constexpr Board board(piece_placement);
  enum_for_each<Color>([](const auto color) {
    enum_for_each<PieceType>([&](const auto piece_type) {
      STATIC_REQUIRE(board.get(color, piece_type) == piece_to_bitboard[color][piece_type]);
    });
    STATIC_REQUIRE(board.all_pieces(color) == [&] {
      Bitboard all_pieces{};
      enum_for_each<PieceType>([&](const auto piece_type) { all_pieces |= board.get(color, piece_type); });
      return all_pieces;
    }());
  });
  STATIC_REQUIRE(board.occupancy() == [] {
    Bitboard occupancy{};
    enum_for_each<Color>([&](const auto color) { occupancy |= board.all_pieces(color); });
    return occupancy;
  }());
}
}  // namespace
}  // namespace prodigy
