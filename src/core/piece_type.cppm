module;

#include <cstdint>

#include "macros.h"

export module prodigy.core:piece_type;

export namespace prodigy {
enum class PieceType : std::uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

PRODIGY_ENUM_IOSTREAM_OPERATORS(PieceType)
}  // namespace prodigy
