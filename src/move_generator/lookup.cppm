module;

#include <magic_enum.hpp>

module prodigy.move_generator:lookup;

import prodigy.core;

import :magic_bitboards;

namespace prodigy::move_generator {
constexpr Bitboard pawn_single_push_set(const Color color, const Bitboard pawns) noexcept {
  switch (color) {
    case Color::WHITE:
      return shift(pawns, Direction::NORTH);
    case Color::BLACK:
      return shift(pawns, Direction::SOUTH);
  }
}

constexpr Bitboard pawn_left_attack_set(const Color color, const Bitboard pawns) noexcept {
  switch (color) {
    case Color::WHITE:
      return shift(pawns, Direction::NORTH_WEST);
    case Color::BLACK:
      return shift(pawns, Direction::SOUTH_EAST);
  }
}

constexpr Bitboard pawn_right_attack_set(const Color color, const Bitboard pawns) noexcept {
  switch (color) {
    case Color::WHITE:
      return shift(pawns, Direction::NORTH_EAST);
    case Color::BLACK:
      return shift(pawns, Direction::SOUTH_WEST);
  }
}

constexpr Bitboard knight_attack_set(const Square origin) noexcept {
  static constexpr auto attack_sets = [] consteval {
    EnumMap<Square, Bitboard> attack_sets{};
    magic_enum::enum_for_each<Square>([&](const auto origin) {
      magic_enum::enum_for_each<Direction>([&](const auto direction) {
        const auto set_targets = [&](const auto... diagonal_directions) {
          attack_sets[origin] |= (shift(shift(to_bitboard(origin), direction), diagonal_directions) | ...);
        };
        switch (direction) {
          case Direction::NORTH:
            set_targets(Direction::NORTH_WEST, Direction::NORTH_EAST);
            break;
          case Direction::EAST:
            set_targets(Direction::NORTH_EAST, Direction::SOUTH_EAST);
            break;
          case Direction::SOUTH:
            set_targets(Direction::SOUTH_EAST, Direction::SOUTH_WEST);
            break;
          case Direction::WEST:
            set_targets(Direction::SOUTH_WEST, Direction::NORTH_WEST);
            break;
          default:
            break;
        }
      });
    });
    return attack_sets;
  }
  ();
  return attack_sets[origin];
}

inline Bitboard bishop_attack_set(const Square origin, const Bitboard occupancy) noexcept {
  return BishopMagicBitboards::attack_set(origin, occupancy);
}

inline Bitboard rook_attack_set(const Square origin, const Bitboard occupancy) noexcept {
  return RookMagicBitboards::attack_set(origin, occupancy);
}

constexpr Bitboard king_attack_set(const Square origin) noexcept {
  static constexpr auto attack_sets = [] consteval {
    EnumMap<Square, Bitboard> attack_sets{};
    magic_enum::enum_for_each<Square>([&](const auto origin) {
      magic_enum::enum_for_each<Direction>(
          [&](const auto direction) { attack_sets[origin] |= shift(to_bitboard(origin), direction); });
    });
    return attack_sets;
  }
  ();
  return attack_sets[origin];
}

constexpr Bitboard half_open_segment(const Square origin, const Square target) noexcept {
  static constexpr auto segments = [] consteval {
    EnumMap<Square, EnumMap<Square, Bitboard>> segments{};
    magic_enum::enum_for_each<Square>([&](const auto origin) {
      magic_enum::enum_for_each<Direction>([&](const auto direction) {
        for (auto segment = Bitboard(), target = to_bitboard(origin); any(target);
             segment |= target, target = shift(target, direction)) {
          segments[origin][square_of(target)] = segment;
        }
      });
    });
    return segments;
  }
  ();
  return segments[origin][target];
}
}  // namespace prodigy::move_generator
