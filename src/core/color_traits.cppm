module;

#include <magic_enum_utility.hpp>

export module prodigy.core:color_traits;

import :bitboard;
import :castling_rights;
import :color;
import :move;
import :square;

namespace prodigy {
namespace {
consteval Bitboard on_mask(const Rank rank) noexcept {
  Bitboard on_mask{};
  magic_enum::enum_for_each<File>([&](const auto file) { on_mask |= to_bitboard(to_square(file, rank)); });
  return on_mask;
}
}  // namespace
}  // namespace prodigy

export namespace prodigy {
template <Color>
struct ColorTraits;

template <>
struct ColorTraits<Color::WHITE> {
  static constexpr auto KINGSIDE_CASTLING_RIGHTS = CastlingRights::WHITE_KINGSIDE;
  static constexpr auto KINGSIDE_CASTLE = Castle{
      .king_origin = to_bitboard(Square::E1),
      .king_target = to_bitboard(Square::G1),
      .rook_origin = to_bitboard(Square::H1),
      .rook_target = to_bitboard(Square::F1),
  };
  static constexpr auto QUEENSIDE_CASTLING_RIGHTS = CastlingRights::WHITE_QUEENSIDE;
  static constexpr auto QUEENSIDE_CASTLE = Castle{
      .king_origin = to_bitboard(Square::E1),
      .king_target = to_bitboard(Square::C1),
      .rook_origin = to_bitboard(Square::A1),
      .rook_target = to_bitboard(Square::D1),
  };
  static constexpr auto CASTLING_RIGHTS = KINGSIDE_CASTLING_RIGHTS | QUEENSIDE_CASTLING_RIGHTS;

  static constexpr auto PROMOTION_RANK = on_mask(Rank::EIGHT);
  static constexpr auto EN_PASSANT_TARGET_RANK = on_mask(Rank::THREE);
};

template <>
struct ColorTraits<Color::BLACK> {
  static constexpr auto KINGSIDE_CASTLING_RIGHTS = CastlingRights::BLACK_KINGSIDE;
  static constexpr auto KINGSIDE_CASTLE = Castle{
      .king_origin = to_bitboard(Square::E8),
      .king_target = to_bitboard(Square::G8),
      .rook_origin = to_bitboard(Square::H8),
      .rook_target = to_bitboard(Square::F8),
  };
  static constexpr auto QUEENSIDE_CASTLING_RIGHTS = CastlingRights::BLACK_QUEENSIDE;
  static constexpr auto QUEENSIDE_CASTLE = Castle{
      .king_origin = to_bitboard(Square::E8),
      .king_target = to_bitboard(Square::C8),
      .rook_origin = to_bitboard(Square::A8),
      .rook_target = to_bitboard(Square::D8),
  };
  static constexpr auto CASTLING_RIGHTS = KINGSIDE_CASTLING_RIGHTS | QUEENSIDE_CASTLING_RIGHTS;

  static constexpr auto PROMOTION_RANK = on_mask(Rank::ONE);
  static constexpr auto EN_PASSANT_TARGET_RANK = on_mask(Rank::SIX);
};
}  // namespace prodigy
