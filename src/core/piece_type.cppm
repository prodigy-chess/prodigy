module;

#include <cstdint>

export module prodigy.core:piece_type;

export namespace prodigy {
enum class PieceType : std::uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
}
