#include <catch2/catch_test_macros.hpp>
#include <utility>

import prodigy.core;

namespace prodigy {
namespace {
TEST_CASE("dispatch") {
  for (std::underlying_type_t<CastlingRights> underlying = 0; underlying < 16; ++underlying) {
    const CastlingRights castling_rights{underlying};
    REQUIRE(&dispatch(castling_rights, [&]<auto dispatched_castling_rights> -> const CastlingRights& {
      REQUIRE(dispatched_castling_rights == castling_rights);
      return castling_rights;
    }) == &castling_rights);
  }
}
}  // namespace
}  // namespace prodigy
