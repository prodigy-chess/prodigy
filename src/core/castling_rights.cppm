module;

#include <cstdint>
#include <magic_enum.hpp>

#include "macros.h"

export module prodigy.core:castling_rights;

export namespace prodigy {
enum class CastlingRights : std::uint8_t {
  WHITE_KINGSIDE = 1 << 0,
  WHITE_QUEENSIDE = 1 << 1,
  BLACK_KINGSIDE = 1 << 2,
  BLACK_QUEENSIDE = 1 << 3,
};

PRODIGY_ENUM_BITWISE_OPERATORS(CastlingRights)
PRODIGY_ENUM_IOSTREAM_OPERATORS(CastlingRights)
}  // namespace prodigy

namespace magic_enum::customize {
template <>
struct enum_range<prodigy::CastlingRights> {
  static constexpr bool is_flags = true;
};
}  // namespace magic_enum::customize
