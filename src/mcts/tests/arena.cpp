#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <cstdint>

import prodigy.mcts;

namespace prodigy::mcts {
namespace {
TEST_CASE("alignment") {
  STATIC_REQUIRE_FALSE(ArenaAllocatable<std::uint8_t>);
  STATIC_REQUIRE_FALSE(ArenaAllocatable<std::uint16_t>);
  STATIC_REQUIRE_FALSE(ArenaAllocatable<std::uint32_t>);
  STATIC_REQUIRE(ArenaAllocatable<std::uint64_t>);
}

TEST_CASE("trivially destructible") {
  struct alignas(alignof(std::uint64_t)) Foo {};
  STATIC_REQUIRE(ArenaAllocatable<Foo>);
  struct alignas(alignof(std::uint64_t)) Bar {
    ~Bar() {}
  };
  STATIC_REQUIRE_FALSE(ArenaAllocatable<Bar>);
}

template <typename T>
concept DefaultConstructible = requires(Arena arena) {
  { arena.new_object<T>() } -> std::same_as<T&>;
};

TEST_CASE("noexcept constructible") {
  struct alignas(alignof(std::uint64_t)) Foo {};
  STATIC_REQUIRE(DefaultConstructible<Foo>);
  struct alignas(alignof(std::uint64_t)) Bar {
    Bar() {}
  };
  STATIC_REQUIRE_FALSE(DefaultConstructible<Bar>);
}

struct Foo {
  static Foo& create(Arena& arena) noexcept {
    auto& object = arena.new_object<Foo>(0);
    REQUIRE(object.foo == 0);
    return object;
  }

  std::uint64_t foo;
};

struct BarBaz {
  static BarBaz& create(Arena& arena) noexcept {
    auto& object = arena.new_object<BarBaz>(0, 1);
    REQUIRE(object.bar == 0);
    REQUIRE(object.baz == 1);
    return object;
  }

  std::uint64_t bar;
  std::uint64_t baz;
};

TEST_CASE("new_object") {
  static constexpr auto bytes = sizeof(BarBaz) * 3;
  Arena arena(bytes);
  for (auto i = 0; i < 2; ++i) {
    Foo::create(arena);
    BarBaz::create(arena);
  }
}

TEST_CASE("rollback") {
  static constexpr auto bytes = sizeof(BarBaz) * 3;
  Arena arena(bytes);
  SECTION("one") {
    auto& foo = Foo::create(arena);
    arena.rollback(foo);
    for (auto i = 0UZ; i < bytes / sizeof(BarBaz); ++i) {
      BarBaz::create(arena);
    }
  }
  SECTION("multiple") {
    BarBaz::create(arena);
    auto& second = BarBaz::create(arena);
    BarBaz::create(arena);
    arena.rollback(second);
    for (auto i = 0UZ; i < (bytes - sizeof(BarBaz)) / sizeof(Foo); ++i) {
      Foo::create(arena);
    }
  }
}

TEST_CASE("reset") {
  static constexpr auto bytes = sizeof(BarBaz) * 2;
  Arena arena(bytes);
  BarBaz::create(arena);
  REQUIRE(arena.reset() == sizeof(BarBaz));
  Foo::create(arena);
  BarBaz::create(arena);
  REQUIRE(arena.reset() == sizeof(Foo) + sizeof(BarBaz));
  arena.rollback(Foo::create(arena));
  REQUIRE(arena.reset() == 0);
  BarBaz::create(arena);
  auto& foo = Foo::create(arena);
  Foo::create(arena);
  arena.rollback(foo);
  REQUIRE(arena.reset() == sizeof(BarBaz));
}
}  // namespace
}  // namespace prodigy::mcts
