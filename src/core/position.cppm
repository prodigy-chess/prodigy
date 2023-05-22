module;

#include <cstdint>
#include <optional>

export module prodigy.core:position;

import :board;
import :castling_rights;
import :color;
import :ply;
import :square;

export namespace prodigy {
struct Position {
  Board board;
  Color side_to_move;
  CastlingRights castling_rights;
  std::optional<File> en_passant_target;
  Ply halfmove_clock;
  std::uint16_t fullmove_number;

  friend constexpr bool operator==(const Position&, const Position&) = default;
};
}  // namespace prodigy
