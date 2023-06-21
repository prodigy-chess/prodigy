module prodigy.move_generator:color_traits;

import prodigy.core;

namespace prodigy::move_generator {
template <Color>
struct ColorTraits;

template <>
struct ColorTraits<Color::WHITE> {
  static constexpr auto KINGSIDE_CASTLING_RIGHTS = CastlingRights::WHITE_KINGSIDE;
  static constexpr auto QUEENSIDE_CASTLING_RIGHTS = CastlingRights::WHITE_QUEENSIDE;
  static constexpr auto KINGSIDE_CASTLE = Castle{
      .king_origin = to_bitboard(Square::E1),
      .king_target = to_bitboard(Square::G1),
      .rook_origin = to_bitboard(Square::H1),
      .rook_target = to_bitboard(Square::F1),
      .side_to_move = Color::WHITE,
  };
  static constexpr auto QUEENSIDE_CASTLE = Castle{
      .king_origin = to_bitboard(Square::E1),
      .king_target = to_bitboard(Square::C1),
      .rook_origin = to_bitboard(Square::A1),
      .rook_target = to_bitboard(Square::D1),
      .side_to_move = Color::WHITE,
  };
};

template <>
struct ColorTraits<Color::BLACK> {
  static constexpr auto KINGSIDE_CASTLING_RIGHTS = CastlingRights::BLACK_KINGSIDE;
  static constexpr auto QUEENSIDE_CASTLING_RIGHTS = CastlingRights::BLACK_QUEENSIDE;
  static constexpr auto KINGSIDE_CASTLE = Castle{
      .king_origin = to_bitboard(Square::E8),
      .king_target = to_bitboard(Square::G8),
      .rook_origin = to_bitboard(Square::H8),
      .rook_target = to_bitboard(Square::F8),
      .side_to_move = Color::BLACK,
  };
  static constexpr auto QUEENSIDE_CASTLE = Castle{
      .king_origin = to_bitboard(Square::E8),
      .king_target = to_bitboard(Square::C8),
      .rook_origin = to_bitboard(Square::A8),
      .rook_target = to_bitboard(Square::D8),
      .side_to_move = Color::BLACK,
  };
};
}  // namespace prodigy::move_generator
