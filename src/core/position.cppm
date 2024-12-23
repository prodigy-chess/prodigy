module;

#include <cstdint>

export module prodigy.core:position;

import :bitboard;
import :board;
import :castling_rights;
import :color;
import :ply;

export namespace prodigy {
struct Position {
  Board board;
  Color side_to_move;
  CastlingRights castling_rights;
  Bitboard en_passant_victim_origin;
  Ply halfmove_clock;
  std::uint16_t fullmove_number;

  friend constexpr bool operator==(const Position&, const Position&) = default;
};
}  // namespace prodigy
