#include <asio/io_context.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <chrono>
#include <cstddef>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

import prodigy.core;
import prodigy.engine;
import prodigy.move_generator;
import prodigy.uci;

namespace prodigy {
namespace {
class MockStrategy : public Strategy {
 public:
  enum class State {
    JOINED,
    SEARCHING,
    STOPPING,
  };

  const Position& position() const noexcept {
    REQUIRE(position_.has_value());
    return *position_;
  }

  State state() const noexcept { return state_; }

 private:
  void start(const Position& position, const std::optional<std::size_t> nodes) override {
    REQUIRE_FALSE(nodes.has_value());
    REQUIRE(std::exchange(state_, State::SEARCHING) == State::JOINED);
    position_.emplace(position);
  }

  bool poll() override {
    REQUIRE(state_ != State::JOINED);
    return true;
  }

  void stop() override { REQUIRE(std::exchange(state_, State::STOPPING) != State::JOINED); }

  std::optional<uci::Move> join() override {
    REQUIRE(std::exchange(state_, State::JOINED) != State::JOINED);
    return std::nullopt;
  }

  std::optional<const Position> position_;
  State state_ = State::JOINED;
};

TEST_CASE("handle") {
  static_cast<void>(move_generator::init());
  asio::io_context io_context(1);
  auto strategy_ptr = std::make_unique<MockStrategy>();
  const auto& strategy = *strategy_ptr;
  Engine engine(io_context, std::move(strategy_ptr), std::chrono::steady_clock::duration::zero());
  const auto [command, expected_fen] = GENERATE(table<std::string, std::string_view>({
      {
          "position startpos",
          STARTING_FEN,
      },
      {
          "position startpos moves e2e4",
          "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1",
      },
      {
          "position startpos moves e2e4 g8f6",
          "rnbqkb1r/pppppppp/5n2/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 1 2",
      },
      {
          "position startpos moves e2e4 g8f6 d2d4",
          "rnbqkb1r/pppppppp/5n2/8/3PP3/8/PPP2PPP/RNBQKBNR b KQkq - 0 2",
      },
      {
          "position startpos moves e2e4 g8f6 d2d4 b8c6",
          "r1bqkb1r/pppppppp/2n2n2/8/3PP3/8/PPP2PPP/RNBQKBNR w KQkq - 1 3",
      },
      {
          "position startpos moves e2e4 g8f6 d2d4 b8c6 e1e2",
          "r1bqkb1r/pppppppp/2n2n2/8/3PP3/8/PPP1KPPP/RNBQ1BNR b kq - 2 3",
      },
      {
          "position startpos moves e2e4 g8f6 d2d4 b8c6 e1e2 c6d4",
          "r1bqkb1r/pppppppp/5n2/8/3nP3/8/PPP1KPPP/RNBQ1BNR w kq - 0 4",
      },
      {
          std::string("position fen ") + KIWIPETE,
          KIWIPETE,
      },
      {
          std::format("position fen {} moves a2a4", KIWIPETE),
          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1",
      },
      {
          std::format("position fen {} moves a2a4 b4a3", KIWIPETE),
          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/p1N2Q1p/1PPBBPPP/R3K2R w KQkq - 0 2",
      },
      {
          std::format("position fen {} moves a2a4 b4a3 e1c1", KIWIPETE),
          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/p1N2Q1p/1PPBBPPP/2KR3R b kq - 1 2",
      },
      {
          std::format("position fen {} moves a2a4 b4a3 e1c1 h8g8", KIWIPETE),
          "r3k1r1/p1ppqpb1/bn2pnp1/3PN3/4P3/p1N2Q1p/1PPBBPPP/2KR3R w q - 2 3",
      },
      {
          std::format("position fen {} moves a2a4 b4a3 e1c1 h8g8 d5d6", KIWIPETE),
          "r3k1r1/p1ppqpb1/bn1Ppnp1/4N3/4P3/p1N2Q1p/1PPBBPPP/2KR3R b q - 0 3",
      },
      {
          std::format("position fen {} moves a2a4 b4a3 e1c1 h8g8 d5d6 e8c8", KIWIPETE),
          "2kr2r1/p1ppqpb1/bn1Ppnp1/4N3/4P3/p1N2Q1p/1PPBBPPP/2KR3R w - - 1 4",
      },
      {
          std::format("position fen {} moves a2a4 b4a3 e1c1 h8g8 d5d6 e8c8 d6e7", KIWIPETE),
          "2kr2r1/p1ppPpb1/bn2pnp1/4N3/4P3/p1N2Q1p/1PPBBPPP/2KR3R b - - 0 4",
      },
      {
          std::format("position fen {} moves a2a4 b4a3 e1c1 h8g8 d5d6 e8c8 d6e7 d8f8", KIWIPETE),
          "2k2rr1/p1ppPpb1/bn2pnp1/4N3/4P3/p1N2Q1p/1PPBBPPP/2KR3R w - - 1 5",
      },
      {
          std::format("position fen {} moves a2a4 b4a3 e1c1 h8g8 d5d6 e8c8 d6e7 d8f8 e7f8q", KIWIPETE),
          "2k2Qr1/p1pp1pb1/bn2pnp1/4N3/4P3/p1N2Q1p/1PPBBPPP/2KR3R b - - 0 5",
      },
  }));
  INFO(command);
  engine.handle(command);
  REQUIRE(strategy.state() == MockStrategy::State::JOINED);
  engine.handle("go infinite");
  REQUIRE(strategy.state() == MockStrategy::State::SEARCHING);
  REQUIRE(strategy.position() == parse_fen(expected_fen).value());
  engine.handle("stop");
  REQUIRE(strategy.state() == MockStrategy::State::STOPPING);
  io_context.run();
  REQUIRE(strategy.state() == MockStrategy::State::JOINED);
}
}  // namespace
}  // namespace prodigy
