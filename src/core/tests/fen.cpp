#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <optional>
#include <string_view>
#include <utility>

import prodigy.core;

namespace prodigy {
namespace {
TEST_CASE("valid FEN") {
  STATIC_REQUIRE(parse_fen("8/8/8/8/8/8/8/8 w - - 0 0").value() == Position{});
  STATIC_REQUIRE(parse_fen(STARTING_FEN).value() ==
                 Position{
                     .board{[] {
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
                     }()},
                     .castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE |
                                        CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE,
                     .fullmove_number = 1,
                 });
  const auto [fen, position] = GENERATE(table<std::string_view, Position>({
      {
          "8/8/8/8/8/8/8/8 b Kq e3 100 5000",
          {
              .side_to_move = Color::BLACK,
              .castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::BLACK_QUEENSIDE,
              .en_passant_target = Square::E3,
              .halfmove_clock = Ply{100},
              .fullmove_number = 5000,
          },
      },
      {
          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 3 12",
          {
              .board{[] {
                EnumMap<Square, std::optional<std::pair<Color, PieceType>>> piece_placement;
                piece_placement[Square::A8].emplace(Color::BLACK, PieceType::ROOK);
                piece_placement[Square::E8].emplace(Color::BLACK, PieceType::KING);
                piece_placement[Square::H8].emplace(Color::BLACK, PieceType::ROOK);
                piece_placement[Square::A7].emplace(Color::BLACK, PieceType::PAWN);
                piece_placement[Square::C7].emplace(Color::BLACK, PieceType::PAWN);
                piece_placement[Square::D7].emplace(Color::BLACK, PieceType::PAWN);
                piece_placement[Square::E7].emplace(Color::BLACK, PieceType::QUEEN);
                piece_placement[Square::F7].emplace(Color::BLACK, PieceType::PAWN);
                piece_placement[Square::G7].emplace(Color::BLACK, PieceType::BISHOP);
                piece_placement[Square::A6].emplace(Color::BLACK, PieceType::BISHOP);
                piece_placement[Square::B6].emplace(Color::BLACK, PieceType::KNIGHT);
                piece_placement[Square::E6].emplace(Color::BLACK, PieceType::PAWN);
                piece_placement[Square::F6].emplace(Color::BLACK, PieceType::KNIGHT);
                piece_placement[Square::G6].emplace(Color::BLACK, PieceType::PAWN);
                piece_placement[Square::D5].emplace(Color::WHITE, PieceType::PAWN);
                piece_placement[Square::E5].emplace(Color::WHITE, PieceType::KNIGHT);
                piece_placement[Square::B4].emplace(Color::BLACK, PieceType::PAWN);
                piece_placement[Square::E4].emplace(Color::WHITE, PieceType::PAWN);
                piece_placement[Square::C3].emplace(Color::WHITE, PieceType::KNIGHT);
                piece_placement[Square::F3].emplace(Color::WHITE, PieceType::QUEEN);
                piece_placement[Square::H3].emplace(Color::BLACK, PieceType::PAWN);
                piece_placement[Square::A2].emplace(Color::WHITE, PieceType::PAWN);
                piece_placement[Square::B2].emplace(Color::WHITE, PieceType::PAWN);
                piece_placement[Square::C2].emplace(Color::WHITE, PieceType::PAWN);
                piece_placement[Square::D2].emplace(Color::WHITE, PieceType::BISHOP);
                piece_placement[Square::E2].emplace(Color::WHITE, PieceType::BISHOP);
                piece_placement[Square::F2].emplace(Color::WHITE, PieceType::PAWN);
                piece_placement[Square::G2].emplace(Color::WHITE, PieceType::PAWN);
                piece_placement[Square::H2].emplace(Color::WHITE, PieceType::PAWN);
                piece_placement[Square::A1].emplace(Color::WHITE, PieceType::ROOK);
                piece_placement[Square::E1].emplace(Color::WHITE, PieceType::KING);
                piece_placement[Square::H1].emplace(Color::WHITE, PieceType::ROOK);
                return piece_placement;
              }()},
              .castling_rights = CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE |
                                 CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE,
              .halfmove_clock = Ply{3},
              .fullmove_number = 12,
          },
      },
  }));
  INFO(fen);
  REQUIRE(parse_fen(fen).value() == position);
}

TEST_CASE("invalid FEN") {
  const auto [fen, error] = GENERATE(table<std::string_view, std::string_view>({
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP// w KQkq - 0 1", "Too few files."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBN w KQkq - 0 1", "Too few files."},
      {"rnbqkbnr/pppppppp/8/8/8/7/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Too few files."},
      {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2 w KQkq - 0 1", "Too few files."},
      {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K1R w KQkq - 0 1", "Too few files."},

      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNRR w KQkq - 0 1", "Too many files."},
      {"rnbqkbnr/pppppppp/8/8/8/18/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Too many files."},
      {"rnbqkbnr/pppppppp/8/8/8/81/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Too many files."},
      {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2RR w KQkq - 0 1", "Too many files."},
      {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K3R w KQkq - 0 1", "Too many files."},

      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP w KQkq - 0 1", "Too few ranks."},
      {" rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Too few ranks."},
      {" w KQkq - 0 1", "Too few ranks."},

      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/PPPPPPPP w KQkq - 0 1", "Too many ranks."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/0 w KQkq - 0 1", "Too many ranks."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/8 w KQkq - 0 1", "Too many ranks."},

      {"rnbqkbnr/pppppppp/8/8/8/08/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Invalid piece."},
      {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K9R w KQkq - 0 1", "Invalid piece."},
      {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2o w KQkq - 0 1", "Invalid piece."},

      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR W KQkq - 0 1", "Invalid active color."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR B KQkq - 0 1", "Invalid active color."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR - KQkq - 0 1", "Invalid active color."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR  KQkq - 0 1", "Invalid active color."},

      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkqp - 0 1", "Invalid castling availability."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w  - 0 1", "Invalid castling availability."},

      {"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f9 0 3", "Invalid en passant target square."},
      {"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f3 0 3", "Invalid en passant target square."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq a 0 1", "Invalid en passant target square."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq A 0 1", "Invalid en passant target square."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq 3 0 1", "Invalid en passant target square."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq 6 0 1", "Invalid en passant target square."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq  0 1", "Invalid en passant target square."},

      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - -1 1", "Invalid halfmove clock."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 65536 1", "Invalid halfmove clock."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -  1", "Invalid halfmove clock."},

      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 -1", "Invalid fullmove number."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 65536", "Invalid fullmove number."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 ", "Invalid fullmove number."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0  ", "Invalid fullmove number."},

      {"", "Too few fields."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", "Too few fields."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0", "Too few fields."},

      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ", "Too many fields."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1  ", "Too many fields."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 \n", "Too many fields."},
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 0", "Too many fields."},
  }));
  INFO(fen);
  const auto result = parse_fen(fen);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() == error);
}
}  // namespace
}  // namespace prodigy
