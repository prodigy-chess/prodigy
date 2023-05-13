module;

#include <cstdint>
#include <utility>

export module prodigy.chess:color;

export namespace prodigy::chess {
enum class Color : std::uint8_t { WHITE, BLACK };

constexpr Color operator~(const Color color) noexcept { return static_cast<Color>(std::to_underlying(color) ^ 1); }
}  // namespace prodigy::chess
