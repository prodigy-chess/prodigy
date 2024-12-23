#include <catch2/catch_test_macros.hpp>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_utility.hpp>
#include <string>
#include <utility>

import prodigy.core;

namespace prodigy {
namespace {
using namespace magic_enum;

TEST_CASE("to_square") {
  enum_for_each<File>([](const auto file) {
    enum_for_each<Rank>([&](const auto rank) {
      STATIC_REQUIRE(to_square(file, rank) ==
                     enum_cast<Square>(std::string{enum_name<file.value>().front(), enum_name<rank.value>().front()}));
    });
  });
}

TEST_CASE("file_of") {
  enum_for_each<Square>([](const auto square) {
    STATIC_REQUIRE(file_of(square) == enum_cast<File>(enum_name<square.value>().substr(0, 1)));
  });
}

TEST_CASE("rank_of") {
  enum_for_each<Square>([](const auto square) {
    STATIC_REQUIRE(rank_of(square) == enum_cast<Rank>(enum_name<square.value>().substr(1)));
  });
}

TEST_CASE("enum_name") {
  enum_for_each<Rank>([](const auto rank) {
    STATIC_REQUIRE(enum_name<rank.value>() == std::string{'1' + std::to_underlying(rank.value)});
  });
}
}  // namespace
}  // namespace prodigy
