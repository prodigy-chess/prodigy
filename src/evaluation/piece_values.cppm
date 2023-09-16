module;

#include <cstdint>

export module prodigy.evaluation:piece_values;

import prodigy.core;

export namespace prodigy::evaluation {
#define _(PHASE, PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE) \
  inline constexpr auto PHASE##_PIECE_VALUES = [] {                               \
    EnumMap<PieceType, std::int16_t> piece_values;                               \
    piece_values[PieceType::PAWN] = PAWN_VALUE;                                   \
    piece_values[PieceType::KNIGHT] = KNIGHT_VALUE;                               \
    piece_values[PieceType::BISHOP] = BISHOP_VALUE;                               \
    piece_values[PieceType::ROOK] = ROOK_VALUE;                                   \
    piece_values[PieceType::QUEEN] = QUEEN_VALUE;                                 \
    piece_values[PieceType::KING] = 0;                                            \
    return piece_values;                                                          \
  }()

_(MIDGAME, 82, 337, 365, 477, 1025);

_(ENDGAME, 94, 281, 297, 512, 936);

#undef _
}  // namespace prodigy::evaluation
