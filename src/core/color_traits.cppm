export module prodigy.core:color_traits;

import :bitboard;
import :castling_rights;
import :color;
import :move;
import :piece_type;
import :square;

export namespace prodigy {
template <Color, PieceType>
struct CastlingTraits;

template <>
struct CastlingTraits<Color::WHITE, PieceType::KING> {
  static constexpr auto CASTLING_RIGHTS = CastlingRights::WHITE_KINGSIDE;
  static constexpr auto CASTLE = Castle{
      .king_origin = to_bitboard(Square::E1),
      .king_target = to_bitboard(Square::G1),
      .rook_origin = to_bitboard(Square::H1),
      .rook_target = to_bitboard(Square::F1),
      .side_to_move = Color::WHITE,
  };
};

template <>
struct CastlingTraits<Color::WHITE, PieceType::QUEEN> {
  static constexpr auto CASTLING_RIGHTS = CastlingRights::WHITE_QUEENSIDE;
  static constexpr auto CASTLE = Castle{
      .king_origin = to_bitboard(Square::E1),
      .king_target = to_bitboard(Square::C1),
      .rook_origin = to_bitboard(Square::A1),
      .rook_target = to_bitboard(Square::D1),
      .side_to_move = Color::WHITE,
  };
};

template <>
struct CastlingTraits<Color::BLACK, PieceType::KING> {
  static constexpr auto CASTLING_RIGHTS = CastlingRights::BLACK_KINGSIDE;
  static constexpr auto CASTLE = Castle{
      .king_origin = to_bitboard(Square::E8),
      .king_target = to_bitboard(Square::G8),
      .rook_origin = to_bitboard(Square::H8),
      .rook_target = to_bitboard(Square::F8),
      .side_to_move = Color::BLACK,
  };
};

template <>
struct CastlingTraits<Color::BLACK, PieceType::QUEEN> {
  static constexpr auto CASTLING_RIGHTS = CastlingRights::BLACK_QUEENSIDE;
  static constexpr auto CASTLE = Castle{
      .king_origin = to_bitboard(Square::E8),
      .king_target = to_bitboard(Square::C8),
      .rook_origin = to_bitboard(Square::A8),
      .rook_target = to_bitboard(Square::D8),
      .side_to_move = Color::BLACK,
  };
};

template <Color color>
struct ColorTraits {
  template <PieceType side>
  using CastlingTraits = CastlingTraits<color, side>;
};
}  // namespace prodigy
