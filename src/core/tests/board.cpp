#include <catch2/catch_test_macros.hpp>
#include <magic_enum_utility.hpp>
#include <optional>
#include <utility>

import prodigy.core;

namespace prodigy {
namespace {
using namespace magic_enum;

TEST_CASE("any") {
  static constexpr Board board;
  enum_for_each<Color>([](const auto color) {
    enum_for_each<PieceType>([&](const auto piece_type) { STATIC_REQUIRE(empty(board[color, piece_type])); });
    STATIC_REQUIRE(empty(board[color]));
  });
  STATIC_REQUIRE(empty(board.occupancy()));
}

TEST_CASE("starting board") {
  static constexpr auto piece_placement = [&] {
    EnumMap<Square, std::optional<std::pair<Color, PieceType>>> piece_placement;
    piece_placement[Square::A1].emplace(Color::WHITE, PieceType::ROOK);
    piece_placement[Square::B1].emplace(Color::WHITE, PieceType::KNIGHT);
    piece_placement[Square::C1].emplace(Color::WHITE, PieceType::BISHOP);
    piece_placement[Square::D1].emplace(Color::WHITE, PieceType::QUEEN);
    piece_placement[Square::E1].emplace(Color::WHITE, PieceType::KING);
    piece_placement[Square::F1].emplace(Color::WHITE, PieceType::BISHOP);
    piece_placement[Square::G1].emplace(Color::WHITE, PieceType::KNIGHT);
    piece_placement[Square::H1].emplace(Color::WHITE, PieceType::ROOK);
    piece_placement[Square::A2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::B2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::C2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::D2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::E2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::F2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::G2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::H2].emplace(Color::WHITE, PieceType::PAWN);
    piece_placement[Square::A7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::B7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::C7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::D7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::E7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::F7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::G7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::H7].emplace(Color::BLACK, PieceType::PAWN);
    piece_placement[Square::A8].emplace(Color::BLACK, PieceType::ROOK);
    piece_placement[Square::B8].emplace(Color::BLACK, PieceType::KNIGHT);
    piece_placement[Square::C8].emplace(Color::BLACK, PieceType::BISHOP);
    piece_placement[Square::D8].emplace(Color::BLACK, PieceType::QUEEN);
    piece_placement[Square::E8].emplace(Color::BLACK, PieceType::KING);
    piece_placement[Square::F8].emplace(Color::BLACK, PieceType::BISHOP);
    piece_placement[Square::G8].emplace(Color::BLACK, PieceType::KNIGHT);
    piece_placement[Square::H8].emplace(Color::BLACK, PieceType::ROOK);
    return piece_placement;
  }();
  static constexpr auto piece_to_bitboard = [&] {
    EnumMap<Color, EnumMap<PieceType, Bitboard>> piece_to_bitboard{};
    enum_for_each<Square>([&](const auto square) {
      if (const auto piece = piece_placement[square]; piece.has_value()) {
        piece_to_bitboard[piece->first][piece->second] |= to_bitboard(square);
      }
    });
    return piece_to_bitboard;
  }();
  static constexpr Board board(piece_placement);
  enum_for_each<Color>([](const auto color) {
    enum_for_each<PieceType>([&](const auto piece_type) {
      STATIC_REQUIRE(board[color, piece_type] == piece_to_bitboard[color][piece_type]);
    });
    STATIC_REQUIRE(board[color] == [&] {
      Bitboard all_pieces{};
      enum_for_each<PieceType>([&](const auto piece_type) { all_pieces |= board[color, piece_type]; });
      return all_pieces;
    }());
  });
  STATIC_REQUIRE(board.occupancy() == [] {
    Bitboard occupancy{};
    enum_for_each<Color>([&](const auto color) { occupancy |= board[color]; });
    return occupancy;
  }());
}

TEST_CASE("quiet move") {
  static constexpr auto before =
      parse_fen("rnbqkbnr/1pp2ppp/3p4/p3p3/4P2P/5P2/PPPP2P1/RNBQKBNR w KQkq - 0 4").value().board;
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::G2),
          .target = to_bitboard(Square::G3),
          .piece_type = PieceType::PAWN,
      });
      return board;
    };
    static constexpr auto pawn_single_push = move(before);
    STATIC_REQUIRE(pawn_single_push ==
                   parse_fen("rnbqkbnr/1pp2ppp/3p4/p3p3/4P2P/5PP1/PPPP4/RNBQKBNR b KQkq - 0 4").value().board);
    STATIC_REQUIRE(move(pawn_single_push) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::G2),
          .target = to_bitboard(Square::G4),
          .piece_type = PieceType::PAWN,
      });
      return board;
    };
    static constexpr auto pawn_double_push = move(before);
    STATIC_REQUIRE(pawn_double_push ==
                   parse_fen("rnbqkbnr/1pp2ppp/3p4/p3p3/4P1PP/5P2/PPPP4/RNBQKBNR b KQkq - 0 4").value().board);
    STATIC_REQUIRE(move(pawn_double_push) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::G1),
          .target = to_bitboard(Square::E2),
          .piece_type = PieceType::KNIGHT,
      });
      return board;
    };
    static constexpr auto knight_move = move(before);
    STATIC_REQUIRE(knight_move ==
                   parse_fen("rnbqkbnr/1pp2ppp/3p4/p3p3/4P2P/5P2/PPPPN1P1/RNBQKB1R b KQkq - 1 4").value().board);
    STATIC_REQUIRE(move(knight_move) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::F1),
          .target = to_bitboard(Square::B5),
          .piece_type = PieceType::BISHOP,
      });
      return board;
    };
    static constexpr auto bishop_move = move(before);
    STATIC_REQUIRE(bishop_move ==
                   parse_fen("rnbqkbnr/1pp2ppp/3p4/pB2p3/4P2P/5P2/PPPP2P1/RNBQK1NR b KQkq - 1 4").value().board);
    STATIC_REQUIRE(move(bishop_move) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::H1),
          .target = to_bitboard(Square::H3),
          .piece_type = PieceType::ROOK,
      });
      return board;
    };
    static constexpr auto rook_move = move(before);
    STATIC_REQUIRE(rook_move ==
                   parse_fen("rnbqkbnr/1pp2ppp/3p4/p3p3/4P2P/5P1R/PPPP2P1/RNBQKBN1 b Qkq - 1 4").value().board);
    STATIC_REQUIRE(move(rook_move) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::D1),
          .target = to_bitboard(Square::E2),
          .piece_type = PieceType::QUEEN,
      });
      return board;
    };
    static constexpr auto queen_move = move(before);
    STATIC_REQUIRE(queen_move ==
                   parse_fen("rnbqkbnr/1pp2ppp/3p4/p3p3/4P2P/5P2/PPPPQ1P1/RNB1KBNR b KQkq - 1 4").value().board);
    STATIC_REQUIRE(move(queen_move) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::E1),
          .target = to_bitboard(Square::F2),
          .piece_type = PieceType::KING,
      });
      return board;
    };
    static constexpr auto king_move = move(before);
    STATIC_REQUIRE(king_move ==
                   parse_fen("rnbqkbnr/1pp2ppp/3p4/p3p3/4P2P/5P2/PPPP1KP1/RNBQ1BNR b kq - 1 4").value().board);
    STATIC_REQUIRE(move(king_move) == before);
  }
}

TEST_CASE("capture") {
  static constexpr auto before =
      parse_fen("r3kb1r/nppb2p1/3p4/1B1np1qp/p1K1p2P/P1P2P1N/RP1P2P1/1NBQR3 w kq - 0 14").value().board;
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::F3),
          .target = to_bitboard(Square::E4),
          .aggressor = PieceType::PAWN,
          .victim = PieceType::PAWN,
      });
      return board;
    };
    static constexpr auto pawn_capture = move(before);
    STATIC_REQUIRE(pawn_capture ==
                   parse_fen("r3kb1r/nppb2p1/3p4/1B1np1qp/p1K1P2P/P1P4N/RP1P2P1/1NBQR3 b kq - 0 14").value().board);
    STATIC_REQUIRE(move(pawn_capture) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::H3),
          .target = to_bitboard(Square::G5),
          .aggressor = PieceType::KNIGHT,
          .victim = PieceType::QUEEN,
      });
      return board;
    };
    static constexpr auto knight_capture = move(before);
    STATIC_REQUIRE(knight_capture ==
                   parse_fen("r3kb1r/nppb2p1/3p4/1B1np1Np/p1K1p2P/P1P2P2/RP1P2P1/1NBQR3 b kq - 0 14").value().board);
    STATIC_REQUIRE(move(knight_capture) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::B5),
          .target = to_bitboard(Square::D7),
          .aggressor = PieceType::BISHOP,
          .victim = PieceType::BISHOP,
      });
      return board;
    };
    static constexpr auto bishop_capture = move(before);
    STATIC_REQUIRE(bishop_capture ==
                   parse_fen("r3kb1r/nppB2p1/3p4/3np1qp/p1K1p2P/P1P2P1N/RP1P2P1/1NBQR3 b kq - 0 14").value().board);
    STATIC_REQUIRE(move(bishop_capture) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::E1),
          .target = to_bitboard(Square::E4),
          .aggressor = PieceType::ROOK,
          .victim = PieceType::PAWN,
      });
      return board;
    };
    static constexpr auto rook_capture = move(before);
    STATIC_REQUIRE(rook_capture ==
                   parse_fen("r3kb1r/nppb2p1/3p4/1B1np1qp/p1K1R2P/P1P2P1N/RP1P2P1/1NBQ4 b kq - 0 14").value().board);
    STATIC_REQUIRE(move(rook_capture) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::D1),
          .target = to_bitboard(Square::A4),
          .aggressor = PieceType::QUEEN,
          .victim = PieceType::PAWN,
      });
      return board;
    };
    static constexpr auto queen_capture = move(before);
    STATIC_REQUIRE(queen_capture ==
                   parse_fen("r3kb1r/nppb2p1/3p4/1B1np1qp/Q1K1p2P/P1P2P1N/RP1P2P1/1NB1R3 b kq - 0 14").value().board);
    STATIC_REQUIRE(move(queen_capture) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .origin = to_bitboard(Square::C4),
          .target = to_bitboard(Square::D5),
          .aggressor = PieceType::KING,
          .victim = PieceType::KNIGHT,
      });
      return board;
    };
    static constexpr auto king_capture = move(before);
    STATIC_REQUIRE(king_capture ==
                   parse_fen("r3kb1r/nppb2p1/3p4/1B1Kp1qp/p3p2P/P1P2P1N/RP1P2P1/1NBQR3 b kq - 0 14").value().board);
    STATIC_REQUIRE(move(king_capture) == before);
  }
}

TEST_CASE("castle") {
  static constexpr auto before = parse_fen("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1").value().board;
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .king_origin = to_bitboard(Square::E1),
          .king_target = to_bitboard(Square::G1),
          .rook_origin = to_bitboard(Square::H1),
          .rook_target = to_bitboard(Square::F1),
      });
      return board;
    };
    static constexpr auto kingside_castle = move(before);
    STATIC_REQUIRE(kingside_castle == parse_fen("r3k2r/1b4bq/8/8/8/8/7B/R4RK1 b kq - 1 1").value().board);
    STATIC_REQUIRE(move(kingside_castle) == before);
  }
  {
    static constexpr auto move = [](Board board) {
      board.apply<Color::WHITE>({
          .king_origin = to_bitboard(Square::E1),
          .king_target = to_bitboard(Square::C1),
          .rook_origin = to_bitboard(Square::A1),
          .rook_target = to_bitboard(Square::D1),
      });
      return board;
    };
    static constexpr auto queenside_castle = move(before);
    STATIC_REQUIRE(queenside_castle == parse_fen("r3k2r/1b4bq/8/8/8/8/7B/2KR3R b kq - 1 1").value().board);
    STATIC_REQUIRE(move(queenside_castle) == before);
  }
}

TEST_CASE("quiet promotion") {
  static constexpr auto before = parse_fen("4k3/1P6/8/8/8/8/K7/8 w - - 0 1").value().board;
  static constexpr auto move = [](Board board, const auto promotion) {
    board.apply<Color::WHITE>(QuietPromotion{
        .origin = to_bitboard(Square::B7),
        .target = to_bitboard(Square::B8),
        .promotion = promotion,
    });
    return board;
  };
  {
    static constexpr auto knight_promotion = move(before, PieceType::KNIGHT);
    STATIC_REQUIRE(knight_promotion == parse_fen("1N2k3/8/8/8/8/8/K7/8 b - - 0 1").value().board);
    STATIC_REQUIRE(move(knight_promotion, PieceType::KNIGHT) == before);
  }
  {
    static constexpr auto bishop_promotion = move(before, PieceType::BISHOP);
    STATIC_REQUIRE(bishop_promotion == parse_fen("1B2k3/8/8/8/8/8/K7/8 b - - 0 1").value().board);
    STATIC_REQUIRE(move(bishop_promotion, PieceType::BISHOP) == before);
  }
  {
    static constexpr auto rook_promotion = move(before, PieceType::ROOK);
    STATIC_REQUIRE(rook_promotion == parse_fen("1R2k3/8/8/8/8/8/K7/8 b - - 0 1").value().board);
    STATIC_REQUIRE(move(rook_promotion, PieceType::ROOK) == before);
  }
  {
    static constexpr auto queen_promotion = move(before, PieceType::QUEEN);
    STATIC_REQUIRE(queen_promotion == parse_fen("1Q2k3/8/8/8/8/8/K7/8 b - - 0 1").value().board);
    STATIC_REQUIRE(move(queen_promotion, PieceType::QUEEN) == before);
  }
}

TEST_CASE("capture promotion") {
  static constexpr auto before = parse_fen("2q1k3/1P6/8/8/8/8/K7/8 w - - 0 1").value().board;
  static constexpr auto move = [](Board board, const auto promotion) {
    board.apply<Color::WHITE>({
        .origin = to_bitboard(Square::B7),
        .target = to_bitboard(Square::C8),
        .promotion = promotion,
        .victim = PieceType::QUEEN,
    });
    return board;
  };
  {
    static constexpr auto knight_promotion = move(before, PieceType::KNIGHT);
    STATIC_REQUIRE(knight_promotion == parse_fen("2N1k3/8/8/8/8/8/K7/8 b - - 0 1").value().board);
    STATIC_REQUIRE(move(knight_promotion, PieceType::KNIGHT) == before);
  }
  {
    static constexpr auto bishop_promotion = move(before, PieceType::BISHOP);
    STATIC_REQUIRE(bishop_promotion == parse_fen("2B1k3/8/8/8/8/8/K7/8 b - - 0 1").value().board);
    STATIC_REQUIRE(move(bishop_promotion, PieceType::BISHOP) == before);
  }
  {
    static constexpr auto rook_promotion = move(before, PieceType::ROOK);
    STATIC_REQUIRE(rook_promotion == parse_fen("2R1k3/8/8/8/8/8/K7/8 b - - 0 1").value().board);
    STATIC_REQUIRE(move(rook_promotion, PieceType::ROOK) == before);
  }
  {
    static constexpr auto queen_promotion = move(before, PieceType::QUEEN);
    STATIC_REQUIRE(queen_promotion == parse_fen("2Q1k3/8/8/8/8/8/K7/8 b - - 0 1").value().board);
    STATIC_REQUIRE(move(queen_promotion, PieceType::QUEEN) == before);
  }
}

TEST_CASE("en passant") {
  static constexpr auto before = parse_fen("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1").value().board;
  static constexpr auto move = [](Board board) {
    board.apply<Color::BLACK>({
        .origin = to_bitboard(Square::C4),
        .target = to_bitboard(Square::D3),
        .victim_origin = to_bitboard(Square::D4),
    });
    return board;
  };
  static constexpr auto after = move(before);
  STATIC_REQUIRE(after == parse_fen("8/8/1k6/2b5/8/3p4/5K2/8 w - - 0 2").value().board);
  STATIC_REQUIRE(move(after) == before);
}
}  // namespace
}  // namespace prodigy
