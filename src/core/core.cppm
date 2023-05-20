module;

#include <magic_enum.hpp>

export module prodigy.core;

export import :bitboard;
export import :castling_rights;
export import :color;
export import :piece_type;
export import :ply;
export import :square;

export namespace prodigy {
using magic_enum::ostream_operators::operator<<;

using magic_enum::bitwise_operators::operator~;
using magic_enum::bitwise_operators::operator|;
using magic_enum::bitwise_operators::operator&;
using magic_enum::bitwise_operators::operator^;
using magic_enum::bitwise_operators::operator|=;
using magic_enum::bitwise_operators::operator&=;
using magic_enum::bitwise_operators::operator^=;
}  // namespace prodigy
