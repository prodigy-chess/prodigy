module;

#include <magic_enum.hpp>

export module prodigy.chess;

export import :color;
export import :square;

export namespace prodigy::chess {
using magic_enum::ostream_operators::operator<<;
}
