#pragma once

#include <iosfwd>
#include <magic_enum_iostream.hpp>
#include <optional>

#define PRODIGY_ENUM_BITWISE_OPERATORS(E)                                                                     \
  constexpr E operator~(const E value) noexcept { return ::magic_enum::bitwise_operators::operator~(value); } \
                                                                                                              \
  constexpr E operator|(const E lhs, const E rhs) noexcept {                                                  \
    return ::magic_enum::bitwise_operators::operator|(lhs, rhs);                                              \
  }                                                                                                           \
                                                                                                              \
  constexpr E operator&(const E lhs, const E rhs) noexcept {                                                  \
    return ::magic_enum::bitwise_operators::operator&(lhs, rhs);                                              \
  }                                                                                                           \
                                                                                                              \
  constexpr E operator^(const E lhs, const E rhs) noexcept {                                                  \
    return ::magic_enum::bitwise_operators::operator^(lhs, rhs);                                              \
  }                                                                                                           \
                                                                                                              \
  constexpr E& operator|=(E& lhs, const E rhs) noexcept {                                                     \
    return ::magic_enum::bitwise_operators::operator|=(lhs, rhs);                                             \
  }                                                                                                           \
                                                                                                              \
  constexpr E& operator&=(E& lhs, const E rhs) noexcept {                                                     \
    return ::magic_enum::bitwise_operators::operator&=(lhs, rhs);                                             \
  }                                                                                                           \
                                                                                                              \
  constexpr E& operator^=(E& lhs, const E rhs) noexcept {                                                     \
    return ::magic_enum::bitwise_operators::operator^=(lhs, rhs);                                             \
  }

#define PRODIGY_ENUM_IOSTREAM_OPERATORS(E)                                                                           \
  template <typename Char, typename Traits>                                                                          \
  std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& is, E& value) {                     \
    return ::magic_enum::istream_operators::operator>>(is, value);                                                   \
  }                                                                                                                  \
                                                                                                                     \
  template <typename Char, typename Traits>                                                                          \
  std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const E value) {                \
    return ::magic_enum::ostream_operators::operator<<(os, value);                                                   \
  }                                                                                                                  \
                                                                                                                     \
  template <typename Char, typename Traits>                                                                          \
  std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const std::optional<E> value) { \
    return ::magic_enum::ostream_operators::operator<<(os, value);                                                   \
  }
