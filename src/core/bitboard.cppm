module;

#include <cstdint>
#include <utility>

export module prodigy.core:bitboard;

import :square;

export namespace prodigy {
enum class Bitboard : std::uint64_t {};

constexpr Bitboard to_bitboard(const Square square) noexcept { return Bitboard{1ULL << std::to_underlying(square)}; }
}  // namespace prodigy
