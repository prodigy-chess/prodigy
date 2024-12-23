module;

#include <magic_enum/magic_enum_containers.hpp>

export module prodigy.core:containers;

export namespace prodigy {
template <typename E, typename T>
using EnumMap = magic_enum::containers::array<E, T>;
}
