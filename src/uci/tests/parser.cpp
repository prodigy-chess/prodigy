#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <utility>

import prodigy.uci;

namespace prodigy::uci {
namespace {
class MockEngine : public Engine {
 public:
  void stop() override { stopped_ = true; }

  bool stopped() const noexcept { return stopped_; }

 private:
  bool stopped_ = false;
};

TEST_CASE("parse") {
  const auto [engine, parser] = [] {
    auto engine = std::make_unique<MockEngine>();
    return std::pair<const MockEngine&, Parser>(*engine, std::move(engine));
  }();
  SECTION("stop") {
    REQUIRE_FALSE(engine.stopped());
    REQUIRE(parser.handle("stop\n"));
    REQUIRE(engine.stopped());
  }
  SECTION("quit") { REQUIRE_FALSE(parser.handle("quit\n")); }
}
}  // namespace
}  // namespace prodigy::uci
