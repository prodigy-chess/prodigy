module;

#include <cstdint>

export module prodigy.evaluation:piece_values;

import prodigy.core;

import :phase;

export namespace prodigy::evaluation {
inline constexpr auto PIECE_VALUES = [] {
  EnumMap<Phase, EnumMap<PieceType, std::int16_t>> piece_values;
#define _(PHASE, PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE) \
  piece_values[PHASE][PieceType::PAWN] = PAWN_VALUE;                              \
  piece_values[PHASE][PieceType::KNIGHT] = KNIGHT_VALUE;                          \
  piece_values[PHASE][PieceType::BISHOP] = BISHOP_VALUE;                          \
  piece_values[PHASE][PieceType::ROOK] = ROOK_VALUE;                              \
  piece_values[PHASE][PieceType::QUEEN] = QUEEN_VALUE;                            \
  piece_values[PHASE][PieceType::KING] = 0

  _(Phase::MIDGAME, 82, 337, 365, 477, 1025);

  _(Phase::ENDGAME, 94, 281, 297, 512, 936);

#undef _
  return piece_values;
}();
}  // namespace prodigy::evaluation
