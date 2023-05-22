module;

#include <utility>

#include "macros.h"

export module prodigy.core:color;

export namespace prodigy {
enum class Color : bool { WHITE, BLACK };

constexpr Color operator!(const Color color) noexcept { return Color{!std::to_underlying(color)}; }

PRODIGY_ENUM_IOSTREAM_OPERATORS(Color)
}  // namespace prodigy
