module;

#include <cstdint>
#include <utility>

#include "macros.h"

export module prodigy.core:bitboard;

import :square;

export namespace prodigy {
enum class Bitboard : std::uint64_t {};

constexpr Bitboard to_bitboard(const Square square) noexcept { return Bitboard{1ULL << std::to_underlying(square)}; }

constexpr bool empty(const Bitboard bitboard) noexcept { return bitboard == Bitboard(); }

PRODIGY_ENUM_BITWISE_OPERATORS(Bitboard)
}  // namespace prodigy
