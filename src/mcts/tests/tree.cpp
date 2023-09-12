#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <utility>

import prodigy.core;
import prodigy.mcts;
import prodigy.move_generator;

namespace prodigy::mcts {
namespace {
TEST_CASE("visit_move") {
  auto visited = false;
  SECTION("quiet move") {
    Edge(QuietMove{
             .origin = to_bitboard(Square::E2),
             .target = to_bitboard(Square::E3),
             .piece_type = PieceType::PAWN,
         })
        .visit_move<Color::WHITE>(visitor{
            [&](const QuietMove&) { REQUIRE_FALSE(std::exchange(visited, true)); },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("enable en passant") {
    Edge(
        QuietMove{
            .origin = to_bitboard(Square::E2),
            .target = to_bitboard(Square::E4),
            .piece_type = PieceType::PAWN,
        },
        Edge::EnableEnPassant())
        .visit_move<Color::WHITE>(visitor{
            [&](const QuietMove&, Edge::EnableEnPassant) { REQUIRE_FALSE(std::exchange(visited, true)); },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("quiet move new castling rights") {
    Edge(
        QuietMove{
            .origin = to_bitboard(Square::A1),
            .target = to_bitboard(Square::B1),
            .piece_type = PieceType::ROOK,
        },
        CastlingRights::WHITE_KINGSIDE)
        .visit_move<Color::WHITE>(visitor{
            [&](const QuietMove&, const CastlingRights child_castling_rights) {
              REQUIRE(child_castling_rights == CastlingRights::WHITE_KINGSIDE);
              REQUIRE_FALSE(std::exchange(visited, true));
            },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("capture") {
    Edge(Capture{
             .origin = to_bitboard(Square::A1),
             .target = to_bitboard(Square::B2),
             .aggressor = PieceType::BISHOP,
             .victim = PieceType::BISHOP,
         })
        .visit_move<Color::WHITE>(visitor{
            [&](const Capture&) { REQUIRE_FALSE(std::exchange(visited, true)); },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("capture new castling rights") {
    Edge(
        Capture{
            .origin = to_bitboard(Square::G2),
            .target = to_bitboard(Square::H1),
            .aggressor = PieceType::BISHOP,
            .victim = PieceType::ROOK,
        },
        CastlingRights::WHITE_QUEENSIDE)
        .visit_move<Color::BLACK>(visitor{
            [&](const Capture&, const CastlingRights child_castling_rights) {
              REQUIRE(child_castling_rights == CastlingRights::WHITE_QUEENSIDE);
              REQUIRE_FALSE(std::exchange(visited, true));
            },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("kingside castle") {
    Edge(ColorTraits<Color::WHITE>::KINGSIDE_CASTLE)
        .visit_move<Color::WHITE>(visitor{
            [&](const KingsideCastle&) { REQUIRE_FALSE(std::exchange(visited, true)); },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("queenside castle") {
    Edge(ColorTraits<Color::BLACK>::QUEENSIDE_CASTLE)
        .visit_move<Color::BLACK>(visitor{
            [&](const QueensideCastle&) { REQUIRE_FALSE(std::exchange(visited, true)); },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("quiet promotion") {
    Edge(QuietPromotion{
             .origin = to_bitboard(Square::E7),
             .target = to_bitboard(Square::E8),
             .promotion = PieceType::QUEEN,
         })
        .visit_move<Color::WHITE>(visitor{
            [&](const QuietPromotion&) { REQUIRE_FALSE(std::exchange(visited, true)); },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("capture promotion") {
    Edge(CapturePromotion{
             .origin = to_bitboard(Square::E7),
             .target = to_bitboard(Square::D8),
             .promotion = PieceType::QUEEN,
             .victim = PieceType::KNIGHT,
         })
        .visit_move<Color::WHITE>(visitor{
            [&](const CapturePromotion&) { REQUIRE_FALSE(std::exchange(visited, true)); },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("capture promotion new castling rights") {
    Edge(
        CapturePromotion{
            .origin = to_bitboard(Square::B2),
            .target = to_bitboard(Square::A1),
            .promotion = PieceType::ROOK,
            .victim = PieceType::ROOK,
        },
        CastlingRights())
        .visit_move<Color::WHITE>(visitor{
            [&](const CapturePromotion&, const CastlingRights child_castling_rights) {
              REQUIRE(empty(child_castling_rights));
              REQUIRE_FALSE(std::exchange(visited, true));
            },
            [](auto&&...) { FAIL(); },
        });
  }
  SECTION("en passant") {
    Edge(EnPassant{
             .origin = to_bitboard(Square::E5),
             .target = to_bitboard(Square::D6),
             .victim_origin = to_bitboard(Square::D5),
         })
        .visit_move<Color::WHITE>(visitor{
            [&](const EnPassant&) { REQUIRE_FALSE(std::exchange(visited, true)); },
            [](auto&&...) { FAIL(); },
        });
  }
  REQUIRE(visited);
}

TEST_CASE("get_or_create_child") {
  Edge edge(QuietMove{
      .origin = to_bitboard(Square::E2),
      .target = to_bitboard(Square::E3),
      .piece_type = PieceType::PAWN,
  });
  Node node_1(0, false);
  {
    Node node_2(0, true);
    auto node_2_deleted = false;
    const auto [child, created] = edge.get_or_create_child(
        [&] -> Node& {
          const auto [child, created] =
              edge.get_or_create_child([&] -> Node& { return node_1; }, [](auto&&...) { FAIL(); });
          REQUIRE(&child == &node_1);
          REQUIRE(created);
          return node_2;
        },
        [&](const auto& node) {
          REQUIRE(&node == &node_2);
          REQUIRE_FALSE(std::exchange(node_2_deleted, true));
        });
    REQUIRE(&child == &node_1);
    REQUIRE_FALSE(created);
    REQUIRE(node_2_deleted);
  }
  {
    const auto [child, created] = edge.get_or_create_child(
        [&] -> Node& {
          FAIL();
          return node_1;
        },
        [](auto&&...) { FAIL(); });
    REQUIRE(&child == &node_1);
    REQUIRE_FALSE(created);
  }
}

TEST_CASE("node") {
  Arena arena(sizeof(Node) * 64);
  {
    arena.new_object<Edge>(QuietMove{
        .origin = to_bitboard(Square::E2),
        .target = to_bitboard(Square::E3),
        .piece_type = PieceType::PAWN,
    });
    arena.new_object<Edge>(QuietMove{
        .origin = to_bitboard(Square::E2),
        .target = to_bitboard(Square::E4),
        .piece_type = PieceType::PAWN,
    });
    auto& node = arena.new_object<Node>(2, false);
    REQUIRE(node.edges().size() == 2);
    REQUIRE(std::as_const(node).edges().size() == 2);
    REQUIRE_FALSE(node.is_check());
  }
  {
    auto& stalemate = arena.new_object<Node>(0, false);
    REQUIRE(stalemate.edges().empty());
    REQUIRE(std::as_const(stalemate).edges().empty());
    REQUIRE_FALSE(stalemate.is_check());
  }
  {
    auto& checkmate = arena.new_object<Node>(0, true);
    REQUIRE(checkmate.edges().empty());
    REQUIRE(std::as_const(checkmate).edges().empty());
    REQUIRE(checkmate.is_check());
  }
}

TEST_CASE("tree") {
  move_generator::init().value();
  {
    static constexpr auto position = parse_fen(KIWIPETE).value();
    Tree tree(position);
    REQUIRE(tree.position() == position);
    REQUIRE(tree.root().edges().size() == 48);
    auto quiet_moves = 0UZ;
    auto enable_en_passants = 0UZ;
    auto quiet_moves_new_castling_rights = 0UZ;
    auto captures = 0UZ;
    auto kingside_castles = 0UZ;
    auto queenside_castles = 0UZ;
    for (const auto& edge : tree.root().edges()) {
      edge.visit_move<Color::WHITE>(visitor{
          [&](const QuietMove&) { ++quiet_moves; },
          [&](const QuietMove&, Edge::EnableEnPassant) { ++enable_en_passants; },
          [&](const QuietMove&, CastlingRights) { ++quiet_moves_new_castling_rights; },
          [&](const Capture&) { ++captures; },
          [&](const KingsideCastle&) { ++kingside_castles; },
          [&](const QueensideCastle&) { ++queenside_castles; },
          [](auto&&...) { FAIL(); },
      });
    }
    REQUIRE(quiet_moves == 30);
    REQUIRE(enable_en_passants == 1);
    REQUIRE(quiet_moves_new_castling_rights == 7);
    REQUIRE(captures == 8);
    REQUIRE(kingside_castles == 1);
    REQUIRE(queenside_castles == 1);
    REQUIRE_FALSE(tree.root().is_check());
  }
  {
    static constexpr auto position = parse_fen("r3kq1r/6P1/8/2Pp4/8/8/8/R3K2R w kq d6 0 1").value();
    Tree tree(position);
    REQUIRE(tree.position() == position);
    REQUIRE(tree.root().edges().size() == 36);
    auto quiet_moves = 0UZ;
    auto captures_new_castling_rights = 0UZ;
    auto quiet_promotions = 0UZ;
    auto capture_promotions = 0UZ;
    auto capture_promotions_new_castling_rights = 0UZ;
    auto en_passants = 0UZ;
    for (const auto& edge : tree.root().edges()) {
      edge.visit_move<Color::WHITE>(visitor{
          [&](const QuietMove&) { ++quiet_moves; },
          [&](const Capture&, CastlingRights) { ++captures_new_castling_rights; },
          [&](const QuietPromotion&) { ++quiet_promotions; },
          [&](const CapturePromotion&) { ++capture_promotions; },
          [&](const CapturePromotion&, CastlingRights) { ++capture_promotions_new_castling_rights; },
          [&](const EnPassant&) { ++en_passants; },
          [](auto&&...) { FAIL(); },
      });
    }
    REQUIRE(quiet_moves == 21);
    REQUIRE(captures_new_castling_rights == 2);
    REQUIRE(quiet_promotions == 4);
    REQUIRE(capture_promotions == 4);
    REQUIRE(capture_promotions_new_castling_rights == 4);
    REQUIRE(en_passants == 1);
    REQUIRE_FALSE(tree.root().is_check());
  }
  {
    static constexpr auto position = parse_fen("kbQ5/8/1K6/8/8/8/8/8 b - - 0 1").value();
    Tree stalemate(position);
    REQUIRE(stalemate.position() == position);
    REQUIRE(stalemate.root().edges().empty());
    REQUIRE_FALSE(stalemate.root().is_check());
  }
  {
    static constexpr auto position = parse_fen("k1Q5/8/1K6/8/8/8/8/8 b - - 0 1").value();
    Tree checkmate(position);
    REQUIRE(checkmate.position() == position);
    REQUIRE(checkmate.root().edges().empty());
    REQUIRE(checkmate.root().is_check());
  }
}
}  // namespace
}  // namespace prodigy::mcts
