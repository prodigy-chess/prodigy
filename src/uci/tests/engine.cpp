#include <asio/io_context.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <optional>
#include <string>
#include <vector>

import prodigy.core;
import prodigy.uci;

namespace prodigy::uci {
namespace {
class MockEngine : public Engine {
 public:
  explicit MockEngine(asio::io_context& io_context) noexcept : Engine(io_context), io_context_(io_context) {}

  const Position& position() const noexcept { return position_; }

  const std::vector<Move>& moves() const noexcept { return moves_; }

  bool stopped() const noexcept { return stopped_; }

  bool has_quit() const noexcept { return io_context_.stopped(); }

 private:
  void set_position(const Position& position) override { position_ = position; }

  void apply(const Move move) override { moves_.push_back(move); }

  void stop() override { stopped_ = true; }

  const asio::io_context& io_context_;
  Position position_;
  std::vector<Move> moves_;
  bool stopped_ = false;
};

TEST_CASE("parse") {
  asio::io_context io_context;
  MockEngine engine(io_context);
  SECTION("position") {
    const auto [command, position, moves] = GENERATE(table<std::string, Position, std::vector<Move>>({
        {
            "position startpos",
            STARTING_POSITION,
            {},
        },
        {
            " position\tstartpos ",
            STARTING_POSITION,
            {},
        },
        {
            "unknown-token unknown-token position startpos moves e2e4",
            STARTING_POSITION,
            {
                {
                    .origin = Square::E2,
                    .target = Square::E4,
                },
            },
        },
        {
            " \fposition\r startpos \tmoves \ve2e4   e7e5 g1f3 ",
            STARTING_POSITION,
            {
                {
                    .origin = Square::E2,
                    .target = Square::E4,
                },
                {
                    .origin = Square::E7,
                    .target = Square::E5,
                },
                {
                    .origin = Square::G1,
                    .target = Square::F3,
                },
            },
        },
        {
            // TODO: use std::format with Clang 17.
            std::string("position fen ") + KIWIPETE,
            parse_fen(KIWIPETE).value(),
            {},
        },
        {
            // TODO: use std::format with Clang 17.
            std::string("position fen ") + KIWIPETE + " moves d5e6 b4c3 e6f7 e8d8 f7f8q",
            parse_fen(KIWIPETE).value(),
            {
                {
                    .origin = Square::D5,
                    .target = Square::E6,
                },
                {
                    .origin = Square::B4,
                    .target = Square::C3,
                },
                {
                    .origin = Square::E6,
                    .target = Square::F7,
                },
                {
                    .origin = Square::E8,
                    .target = Square::D8,
                },
                {
                    .origin = Square::F7,
                    .target = Square::F8,
                    .promotion = PieceType::QUEEN,
                },
            },
        },
    }));
    INFO(command);
    REQUIRE(engine.moves().empty());
    engine.handle(command);
    REQUIRE(engine.position() == position);
    REQUIRE_THAT(engine.moves(), Catch::Matchers::Equals(moves));
  }
  SECTION("stop") {
    REQUIRE_FALSE(engine.stopped());
    engine.handle("stop");
    REQUIRE(engine.stopped());
  }
  SECTION("quit") {
    REQUIRE_FALSE(engine.has_quit());
    engine.handle("quit");
    REQUIRE(engine.has_quit());
  }
}
}  // namespace
}  // namespace prodigy::uci
