module;

#include <cstdint>
#include <magic_enum/magic_enum.hpp>
#include <utility>

#include "macros.h"

export module prodigy.core:castling_rights;

export namespace prodigy {
enum class CastlingRights : std::uint8_t {
  WHITE_KINGSIDE = 1 << 0,
  WHITE_QUEENSIDE = 1 << 1,
  BLACK_KINGSIDE = 1 << 2,
  BLACK_QUEENSIDE = 1 << 3,
};

constexpr decltype(auto) dispatch(const CastlingRights castling_rights, auto&& callback) {
  switch (std::to_underlying(castling_rights)) {
#define _(CASTLING_RIGHTS) \
  case CASTLING_RIGHTS:    \
    return std::forward<decltype(callback)>(callback).template operator()<CastlingRights{CASTLING_RIGHTS}>()
    _(0);
    _(1);
    _(2);
    _(3);
    _(4);
    _(5);
    _(6);
    _(7);
    _(8);
    _(9);
    _(10);
    _(11);
    _(12);
    _(13);
    _(14);
    _(15);
#undef _
    default:
      std::unreachable();
  }
}

PRODIGY_ENUM_BITWISE_OPERATORS(CastlingRights)
PRODIGY_ENUM_IOSTREAM_OPERATORS(CastlingRights)
}  // namespace prodigy

namespace magic_enum::customize {
template <>
struct enum_range<prodigy::CastlingRights> {
  static constexpr bool is_flags = true;
};
}  // namespace magic_enum::customize
