#include <catch2/catch_test_macros.hpp>
#include <string_view>

import prodigy.core;
import prodigy.evaluation;

namespace prodigy::evaluation {
namespace {
template <Color side_to_move>
consteval float on_moves(Evaluator& evaluator) noexcept {
  return evaluator.evaluate<side_to_move>();
}

template <Color side_to_move>
consteval float on_moves(Evaluator& evaluator, const auto& move, const auto&... moves) noexcept {
  evaluator.on_move<side_to_move>(move);
  return on_moves<!side_to_move>(evaluator, moves...);
}

consteval float evaluate(const std::string_view fen, const auto&... moves) noexcept {
  const auto position = parse_fen(fen).value();
  Evaluator evaluator;
  evaluator.on_search_start(position.board);
  evaluator.on_simulation_start();
  switch (position.side_to_move) {
    case Color::WHITE:
      return on_moves<Color::WHITE>(evaluator, moves...);
    case Color::BLACK:
      return on_moves<Color::BLACK>(evaluator, moves...);
  }
}

TEST_CASE("no moves") { STATIC_REQUIRE(evaluate(STARTING_FEN) == 0); }

TEST_CASE("early promotion") {
  STATIC_REQUIRE(evaluate("rnbqQbnr/pppp1ppp/4k3/8/8/8/PPP1PPPP/RNBQKBNR b KQ - 0 1") == -1144);
}

TEST_CASE("checkmate") { STATIC_REQUIRE(evaluate("3k3R/R7/8/8/8/8/8/4K3 b - - 0 1") == -1033.5); }

TEST_CASE("quiet move") {
  STATIC_REQUIRE(evaluate(STARTING_FEN,
                          QuietMove{
                              .origin = to_bitboard(Square::E2),
                              .target = to_bitboard(Square::E4),
                              .piece_type = PieceType::PAWN,
                          },
                          QuietMove{
                              .origin = to_bitboard(Square::B8),
                              .target = to_bitboard(Square::C6),
                              .piece_type = PieceType::KNIGHT,
                          }) == evaluate("r1bqkbnr/pppppppp/2n5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
}

TEST_CASE("capture") {
  STATIC_REQUIRE(evaluate("3qk3/8/8/8/8/8/8/3QK3 b - - 0 1", Capture{
                                                                 .origin = to_bitboard(Square::D8),
                                                                 .target = to_bitboard(Square::D1),
                                                                 .aggressor = PieceType::QUEEN,
                                                                 .victim = PieceType::QUEEN,
                                                             }) == evaluate("4k3/8/8/8/8/8/8/3qK3 w - - 0 1"));
}

TEST_CASE("castle") {
  STATIC_REQUIRE(evaluate("r3kbnr/pppqpppp/2n1b3/3p4/4P3/3B1N2/PPPP1PPP/RNBQK2R w KQkq - 0 1",
                          ColorTraits<Color::WHITE>::KINGSIDE_CASTLE, ColorTraits<Color::BLACK>::QUEENSIDE_CASTLE) ==
                 evaluate("2kr1bnr/pppqpppp/2n1b3/3p4/4P3/3B1N2/PPPP1PPP/RNBQ1RK1 w - - 0 1"));
}

TEST_CASE("quiet promotion") {
  STATIC_REQUIRE(evaluate("8/P5k1/8/8/8/8/1K1p4/8 b - - 0 1",
                          QuietPromotion{
                              .origin = to_bitboard(Square::D2),
                              .target = to_bitboard(Square::D1),
                              .promotion = PieceType::ROOK,
                          },
                          QuietPromotion{
                              .origin = to_bitboard(Square::A7),
                              .target = to_bitboard(Square::A8),
                              .promotion = PieceType::QUEEN,
                          }) == evaluate("Q7/6k1/8/8/8/8/1K6/3r4 b - - 0 1"));
}

TEST_CASE("capture promotion") {
  STATIC_REQUIRE(evaluate("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0 1",
                          CapturePromotion{
                              .origin = to_bitboard(Square::D7),
                              .target = to_bitboard(Square::C8),
                              .promotion = PieceType::BISHOP,
                              .victim = PieceType::BISHOP,
                          }) == evaluate("rnBq1k1r/pp2bppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R b KQ - 0 1"));
}

TEST_CASE("en passant") {
  STATIC_REQUIRE(evaluate(STARTING_FEN,
                          QuietMove{
                              .origin = to_bitboard(Square::E2),
                              .target = to_bitboard(Square::E4),
                              .piece_type = PieceType::PAWN,
                          },
                          QuietMove{
                              .origin = to_bitboard(Square::D7),
                              .target = to_bitboard(Square::D5),
                              .piece_type = PieceType::PAWN,
                          },
                          Capture{
                              .origin = to_bitboard(Square::E4),
                              .target = to_bitboard(Square::D5),
                              .aggressor = PieceType::PAWN,
                              .victim = PieceType::PAWN,
                          },
                          QuietMove{
                              .origin = to_bitboard(Square::E7),
                              .target = to_bitboard(Square::E5),
                              .piece_type = PieceType::PAWN,
                          },
                          EnPassant{
                              .origin = to_bitboard(Square::D5),
                              .target = to_bitboard(Square::E6),
                              .victim_origin = to_bitboard(Square::E5),
                          }) == evaluate("rnbqkbnr/ppp2ppp/4P3/8/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"));
}

TEST_CASE("reset") {
  STATIC_REQUIRE(evaluate(KIWIPETE) == [] {
    Evaluator evaluator;
    evaluator.on_search_start(parse_fen(KIWIPETE).value().board);
    evaluator.on_simulation_start();
    evaluator.on_move<Color::WHITE>(Capture{
        .origin = to_bitboard(Square::F3),
        .target = to_bitboard(Square::F6),
        .aggressor = PieceType::QUEEN,
        .victim = PieceType::KNIGHT,
    });
    evaluator.on_simulation_start();
    return evaluator.evaluate<Color::WHITE>();
  }());
}
}  // namespace
}  // namespace prodigy::evaluation
