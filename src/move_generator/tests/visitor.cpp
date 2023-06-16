#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <string_view>

import prodigy.core;
import prodigy.move_generator;

namespace prodigy::move_generator {
namespace {
template <Node::Context context>
constexpr Position to_position(const Node& node, const Ply halfmove_clock) noexcept {
  return {
      .board = node.board,
      .side_to_move = context.side_to_move,
      .castling_rights = context.castling_rights,
      .en_passant_target =
          context.has_en_passant_target ? std::optional(square_of(node.en_passant_target)) : std::nullopt,
      .halfmove_clock = halfmove_clock,
      .fullmove_number = 1,
  };
}

class Visitor : public move_generator::Visitor<Visitor> {
 public:
  constexpr explicit Visitor(Node& node) noexcept : node_(node) {}

  template <Node::Context context>
  constexpr void visit_pawn_move(const QuietMove& double_push, const Bitboard en_passant_target) const noexcept {
    visit<context>("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1", double_push, Ply{0},
                   en_passant_target);
  }

  template <Node::Context context, typename Move>
  constexpr void visit_pawn_move(const Move& move) const noexcept {
    visit<context>("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1P/PPPBBP1P/R3K2R b KQkq - 0 1", move, Ply{0});
  }

  template <Node::Context context, typename Move>
  constexpr void visit_knight_move(const Move& move) const noexcept {
    visit<context>("r3k2r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1", move, Ply{0});
  }

  template <Node::Context context, typename Move>
  constexpr void visit_bishop_move(const Move& move) const noexcept {
    visit<context>("r3k2r/p1ppqpb1/Bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1", move, Ply{0});
  }

  template <Node::Context context, typename Move>
  constexpr void visit_rook_move(const Move& move) const noexcept {
    visit<context>("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/1R2K2R b Kkq - 1 1", move, Ply{1});
  }

  template <Node::Context context, typename Move>
  constexpr void visit_queen_move(const Move& move) const noexcept {
    visit<context>("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N4Q/PPPBBPPP/R3K2R b KQkq - 0 1", move, Ply{0});
  }

  template <Node::Context context, typename Move>
  constexpr void visit_king_move(const Move& move) const noexcept {
    visit<context>("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4RK1 b kq - 1 1", move, Ply{1});
  }

 private:
  template <Node::Context context, typename Move>
  constexpr void visit(const std::string_view fen, const Move& move, const Ply halfmove_clock,
                       const Bitboard en_passant_target = Bitboard()) const noexcept {
    INFO(fen);
    const auto undo = scoped_move(node_, move, en_passant_target);
    REQUIRE(to_position<context>(node_, halfmove_clock) == parse_fen(fen).value());
  }

  Node& node_;
};

TEST_CASE("scoped_move") {
  const auto position = parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1").value();
  dispatch(position, [&]<auto context>(auto node) {
    const Visitor visitor(node);
    visitor.visit_pawn_move<context.move(Node::Context::Move::PAWN_DOUBLE_PUSH)>(
        QuietMove{
            .origin = to_bitboard(Square::A2),
            .target = to_bitboard(Square::A4),
            .side_to_move = Color::WHITE,
            .piece_type = PieceType::PAWN,
        },
        to_bitboard(Square::A3));
    REQUIRE(to_position<context>(node, Ply{0}) == position);
    visitor.visit_pawn_move<context.move()>(Capture{
        .origin = to_bitboard(Square::G2),
        .target = to_bitboard(Square::H3),
        .side_to_move = Color::WHITE,
        .aggressor = PieceType::PAWN,
        .victim = PieceType::PAWN,
    });
    REQUIRE(to_position<context>(node, Ply{0}) == position);
    visitor.visit_knight_move<context.move()>(Capture{
        .origin = to_bitboard(Square::E5),
        .target = to_bitboard(Square::F7),
        .side_to_move = Color::WHITE,
        .aggressor = PieceType::KNIGHT,
        .victim = PieceType::PAWN,
    });
    REQUIRE(to_position<context>(node, Ply{0}) == position);
    visitor.visit_bishop_move<context.move()>(Capture{
        .origin = to_bitboard(Square::E2),
        .target = to_bitboard(Square::A6),
        .side_to_move = Color::WHITE,
        .aggressor = PieceType::BISHOP,
        .victim = PieceType::BISHOP,
    });
    REQUIRE(to_position<context>(node, Ply{0}) == position);
    visitor.visit_rook_move<context.move(Node::Context::Move::QUEENSIDE_ROOK_MOVE)>(QuietMove{
        .origin = to_bitboard(Square::A1),
        .target = to_bitboard(Square::B1),
        .side_to_move = Color::WHITE,
        .piece_type = PieceType::ROOK,
    });
    REQUIRE(to_position<context>(node, Ply{0}) == position);
    visitor.visit_queen_move<context.move()>(Capture{
        .origin = to_bitboard(Square::F3),
        .target = to_bitboard(Square::H3),
        .side_to_move = Color::WHITE,
        .aggressor = PieceType::QUEEN,
        .victim = PieceType::PAWN,
    });
    REQUIRE(to_position<context>(node, Ply{0}) == position);
    visitor.visit_king_move<context.move(Node::Context::Move::KING_MOVE)>(Castle{
        .king_origin = to_bitboard(Square::E1),
        .king_target = to_bitboard(Square::G1),
        .rook_origin = to_bitboard(Square::H1),
        .rook_target = to_bitboard(Square::F1),
        .side_to_move = Color::WHITE,
    });
    REQUIRE(to_position<context>(node, Ply{0}) == position);
  });
}
}  // namespace
}  // namespace prodigy::move_generator
