module;

#include <cstdint>

export module prodigy.evaluation:phase;

import prodigy.core;

export namespace prodigy::evaluation {
enum class Phase : std::uint8_t { MIDGAME, ENDGAME };

inline constexpr auto PHASE_DELTAS = [] {
  EnumMap<PieceType, std::int16_t> phase_deltas;
  phase_deltas[PieceType::PAWN] = 0;
  phase_deltas[PieceType::KNIGHT] = 1;
  phase_deltas[PieceType::BISHOP] = 1;
  phase_deltas[PieceType::ROOK] = 2;
  phase_deltas[PieceType::QUEEN] = 4;
  phase_deltas[PieceType::KING] = 0;
  return phase_deltas;
}();
}  // namespace prodigy::evaluation
