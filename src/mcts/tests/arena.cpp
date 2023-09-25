#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <concepts>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <utility>

import prodigy.mcts;

namespace prodigy::mcts {
namespace {
TEST_CASE("constructor") {
  using namespace Catch::Matchers;
  REQUIRE_THROWS_MATCHES(Arena(Arena::ALIGNMENT + 1), std::runtime_error,
                         MessageMatches(Matches((std::ostringstream() << "\\d+ byte arena at \\w+ doesn't support "
                                                                      << Arena::ALIGNMENT << " byte alignment")
                                                    .str())));
}

TEST_CASE("move") {
  static constexpr auto value = 42ULL;
  Arena old_arena(sizeof(value));
  REQUIRE(old_arena.size() == 0);
  REQUIRE(old_arena.new_object<decltype(value)>(value) == value);
  REQUIRE(old_arena.size() == sizeof(value));
  Arena new_arena(std::move(old_arena));
  REQUIRE(new_arena.size() == sizeof(value));
  REQUIRE(old_arena.size() == 0);
  old_arena.reset(old_arena.size());
  REQUIRE(old_arena.size() == 0);
}

template <typename T>
concept DefaultConstructible = requires(Arena arena) {
  { arena.new_object<T>() } -> std::same_as<T&>;
};

TEST_CASE("constraints") {
  struct alignas(Arena::ALIGNMENT / 2) Foo {};
  STATIC_REQUIRE_FALSE(DefaultConstructible<Foo>);
  struct alignas(Arena::ALIGNMENT) Bar {
    ~Bar() {}
  };
  STATIC_REQUIRE_FALSE(DefaultConstructible<Bar>);
  struct alignas(Arena::ALIGNMENT) Baz {
    Baz() {}
  };
  STATIC_REQUIRE_FALSE(DefaultConstructible<Baz>);
}

struct alignas(Arena::ALIGNMENT) Foo {
  static Foo& create(Arena& arena) noexcept {
    const auto size = arena.size();
    auto& object = arena.new_object<Foo>(0);
    REQUIRE(object.foo == 0);
    REQUIRE(arena.size() == size + sizeof(object));
    return object;
  }

  std::uint64_t foo;
};

struct alignas(Arena::ALIGNMENT) BarBaz {
  static BarBaz& create(Arena& arena) noexcept {
    const auto size = arena.size();
    auto& object = arena.new_object<BarBaz>(0, 1);
    REQUIRE(object.bar == 0);
    REQUIRE(object.baz == 1);
    REQUIRE(arena.size() == size + sizeof(object));
    return object;
  }

  std::uint64_t bar;
  std::uint64_t baz;
};

TEST_CASE("new_object") {
  static constexpr auto bytes = sizeof(BarBaz) * 3;
  Arena arena(bytes);
  REQUIRE(arena.size() == 0);
  for (auto i = 0; i < 2; ++i) {
    Foo::create(arena);
    BarBaz::create(arena);
  }
  REQUIRE(arena.size() == bytes);
}

TEST_CASE("rollback") {
  static constexpr auto bytes = sizeof(BarBaz) * 3;
  Arena arena(bytes);
  REQUIRE(arena.size() == 0);

  Foo::create(arena);
  arena.reset(sizeof(Foo));
  REQUIRE(arena.size() == 0);

  Foo::create(arena);
  Foo::create(arena);
  BarBaz::create(arena);
  arena.reset(sizeof(BarBaz) + sizeof(Foo));
  REQUIRE(arena.size() == sizeof(Foo));

  Foo::create(arena);
  BarBaz::create(arena);
  BarBaz::create(arena);
  arena.reset(arena.size());
  REQUIRE(arena.size() == 0);
}
}  // namespace
}  // namespace prodigy::mcts
