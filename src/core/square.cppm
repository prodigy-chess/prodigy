module;

#include <cstdint>
#include <magic_enum/magic_enum.hpp>
#include <utility>

#include "macros.h"

export module prodigy.core:square;

export namespace prodigy {
// clang-format off
enum class Square : std::uint8_t {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
};
// clang-format on

enum class File : std::uint8_t { A, B, C, D, E, F, G, H };

enum class Rank : std::uint8_t { ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT };

constexpr Square to_square(const File file, const Rank rank) noexcept {
  return static_cast<Square>(std::to_underlying(rank) * magic_enum::enum_count<File>() + std::to_underlying(file));
}

constexpr File file_of(const Square square) noexcept {
  return static_cast<File>(std::to_underlying(square) % magic_enum::enum_count<File>());
}

constexpr Rank rank_of(const Square square) noexcept {
  return static_cast<Rank>(std::to_underlying(square) / magic_enum::enum_count<File>());
}

PRODIGY_ENUM_IOSTREAM_OPERATORS(Square)
PRODIGY_ENUM_IOSTREAM_OPERATORS(File)
PRODIGY_ENUM_IOSTREAM_OPERATORS(Rank)
}  // namespace prodigy

namespace magic_enum::customize {
template <>
constexpr customize_t enum_name<prodigy::Rank>(const prodigy::Rank rank) noexcept {
  switch (rank) {
    using enum prodigy::Rank;
    case ONE:
      return "1";
    case TWO:
      return "2";
    case THREE:
      return "3";
    case FOUR:
      return "4";
    case FIVE:
      return "5";
    case SIX:
      return "6";
    case SEVEN:
      return "7";
    case EIGHT:
      return "8";
  }
  return invalid_tag;
}
}  // namespace magic_enum::customize
