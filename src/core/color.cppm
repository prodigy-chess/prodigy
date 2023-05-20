module;

#include <utility>

export module prodigy.core:color;

export namespace prodigy {
enum class Color : bool { WHITE, BLACK };

constexpr Color operator!(const Color color) noexcept { return Color{!std::to_underlying(color)}; }
}  // namespace prodigy
