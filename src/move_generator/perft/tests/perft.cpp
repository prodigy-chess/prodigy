#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstdint>
#include <string_view>
#include <utility>

import prodigy.core;
import prodigy.move_generator;
import prodigy.move_generator.perft;

namespace prodigy::move_generator::perft {
namespace {
TEST_CASE("perft") {
  static constexpr auto KIWIPETE = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
#ifdef NDEBUG
  const auto [fen, depth, leaf_node_count] = GENERATE(table<std::string_view, Ply, std::uint64_t>({
      {STARTING_FEN, Ply{7}, 3'195'901'860},
      {KIWIPETE, Ply{6}, 8'031'647'685},
      {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", Ply{8}, 3'009'794'393},
      {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", Ply{6}, 706'045'033},
      {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", Ply{5}, 89'941'194},
      {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", Ply{6}, 6'923'051'137},
      {"rnbQkbnr/3ppppp/p1p5/8/8/2P5/PP1PPPPP/RNB1KBNR b KQkq - 0 4", Ply{7}, 67'820'026},
      {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", Ply{6}, 71'179'139},
      {"8/ppp3p1/8/8/3p4/5Q2/1ppp2K1/brk4n w - - 0 1", Ply{4}, 134'167},
      {"8/6kR/8/8/8/bq6/1rqqqqqq/K1nqnbrq b - - 0 1", Ply{4}, 50'268},
      {"8/8/8/k7/2p5/8/3P4/4b2K w - - 0 1", Ply{6}, 318'300},             // illegal unpinned en passant in check
      {"3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", Ply{6}, 1'134'888},           // illegal unpinned en passant into check
      {"8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1", Ply{6}, 1'015'133},          // illegal pinned en passant
      {"8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", Ply{6}, 1'440'467},         // en passant into check
      {"5k2/8/8/8/8/8/8/4K2R w K - 0 1", Ply{6}, 661'072},                // kingside castle into check
      {"3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", Ply{6}, 803'711},                // queenside castle into check
      {"r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", Ply{4}, 1'274'206},   // all castling rights
      {"r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", Ply{4}, 1'720'476},    // illegal castle
      {"8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", Ply{5}, 1'004'658},         // discover check
      {"1b1q1n1r/2P1P1P1/4K3/8/8/8/8/3k4 w - - 0 1", Ply{5}, 4'770'774},  // promote out of check
      {"4k3/1P6/8/8/8/8/K7/8 w - - 0 1", Ply{6}, 217'342},                // promote into check
      {"8/P1k5/K7/8/8/8/8/8 w - - 0 1", Ply{6}, 92'683},                  // underpromote into check
      {"K1k5/8/P7/8/8/8/8/8 w - - 0 1", Ply{6}, 2'217},                   // self stalemate
      {"8/k1P5/8/1K6/8/8/8/8 w - - 0 1", Ply{7}, 567'584},                // promote into stalement and checkmate
      {"8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", Ply{4}, 235'27},              // stalemate and checkmate
  }));
#else
  const auto [fen, depth, leaf_node_count] = GENERATE(table<std::string_view, Ply, std::uint64_t>({
      {STARTING_FEN, Ply{5}, 4'865'609},
      {KIWIPETE, Ply{5}, 193'690'690},
  }));
#endif
  static_cast<void>(init());
  INFO(fen);
  INFO("depth " << std::to_underlying(depth));
  REQUIRE(perft(parse_fen(fen).value(), depth).value() == leaf_node_count);
}

TEST_CASE("invalid") {
  const auto result = perft(parse_fen(STARTING_FEN).value(), Ply{0});
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() == "Invalid depth.");
}
}  // namespace
}  // namespace prodigy::move_generator::perft
