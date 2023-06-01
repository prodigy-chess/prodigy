module;

#include <cstdint>

#include "macros.h"

export module prodigy.core:direction;

export namespace prodigy {
enum class Direction : std::int8_t {
  NORTH = 8,
  EAST = 1,
  SOUTH = -NORTH,
  WEST = -EAST,
  NORTH_EAST = NORTH + EAST,
  SOUTH_EAST = SOUTH + EAST,
  SOUTH_WEST = SOUTH + WEST,
  NORTH_WEST = NORTH + WEST,
};

PRODIGY_ENUM_IOSTREAM_OPERATORS(Direction)
}  // namespace prodigy
