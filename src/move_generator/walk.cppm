module;

#include <functional>
#include <utility>

export module prodigy.move_generator:walk;

import prodigy.core;

import :lookup;
import :node;
import :visitor;

namespace prodigy::move_generator {
template <Color side_to_move>
Bitboard make_king_danger_set(const Board& board) noexcept {
  auto king_danger_set = pawn_left_attack_set(!side_to_move, board[!side_to_move, PieceType::PAWN]) |
                         pawn_right_attack_set(!side_to_move, board[!side_to_move, PieceType::PAWN]);
  for_each_square(board[!side_to_move, PieceType::KNIGHT],
                  [&](const auto origin) { king_danger_set |= knight_attack_set(origin); });
  const auto kingless_occupancy = board.occupancy() ^ board[side_to_move, PieceType::KING];
  for_each_square(board[!side_to_move, PieceType::BISHOP] | board[!side_to_move, PieceType::QUEEN],
                  [&](const auto origin) { king_danger_set |= bishop_attack_set(origin, kingless_occupancy); });
  for_each_square(board[!side_to_move, PieceType::ROOK] | board[!side_to_move, PieceType::QUEEN],
                  [&](const auto origin) { king_danger_set |= rook_attack_set(origin, kingless_occupancy); });
  return king_danger_set;
}

template <Color side_to_move>
constexpr PieceType capture_victim_at(const Board& board, const Bitboard target) noexcept {
  // FIXME: try unrolling.
  for (const auto piece_type : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK}) {
    if (any(board[!side_to_move, piece_type] & target)) {
      return piece_type;
    }
  }
  return PieceType::QUEEN;
}

template <Color side_to_move, PieceType piece_type, typename VisitMove>
constexpr void walk_quiet_moves_and_captures(const Board& board, const Bitboard origin, const Bitboard attack_set,
                                             VisitMove&& visit_move) {
  for_each_bit(attack_set & ~board.occupancy(), [&](const auto target) {
    std::invoke(std::forward<VisitMove>(visit_move), QuietMove{
                                                         .origin = origin,
                                                         .target = target,
                                                         .side_to_move = side_to_move,
                                                         .piece_type = piece_type,
                                                     });
  });
  for_each_bit(attack_set & board[!side_to_move], [&](const auto target) {
    std::invoke(std::forward<VisitMove>(visit_move), Capture{
                                                         .origin = origin,
                                                         .target = target,
                                                         .side_to_move = side_to_move,
                                                         .aggressor = piece_type,
                                                         .victim = capture_victim_at<side_to_move>(board, target),
                                                     });
  });
}

template <Color side_to_move, CastlingRights castling_rights, PieceType side, typename VisitMove>
constexpr void walk_castle(const Bitboard king_danger_set, VisitMove&& visit_move) {
  if constexpr (using CastlingTraits = ColorTraits<side_to_move>::template CastlingTraits<side>;
                any(castling_rights & CastlingTraits::CASTLING_RIGHTS)) {
    if (static constexpr auto& castle = CastlingTraits::CASTLE;
        !any(king_danger_set & (castle.king_origin | castle.rook_target | castle.king_target))) {
      std::invoke(std::forward<VisitMove>(visit_move), castle);
    }
  }
}

template <Color side_to_move, CastlingRights castling_rights, typename VisitMove>
constexpr void walk_king_moves(const Board& board, const Bitboard king_danger_set, VisitMove&& visit_move) {
  const auto origin = board[side_to_move, PieceType::KING];
  walk_quiet_moves_and_captures<side_to_move, PieceType::KING>(
      board, origin, king_attack_set(square_of(origin)) & ~king_danger_set, std::forward<VisitMove>(visit_move));
  walk_castle<side_to_move, castling_rights, PieceType::KING>(king_danger_set, std::forward<VisitMove>(visit_move));
  walk_castle<side_to_move, castling_rights, PieceType::QUEEN>(king_danger_set, std::forward<VisitMove>(visit_move));
}
}  // namespace prodigy::move_generator

export namespace prodigy::move_generator {
template <Node::Context context, typename T>
void walk(const Node& node, Visitor<T>&& visitor) {
  const auto king_danger_set = make_king_danger_set<context.side_to_move>(node.board);
  walk_king_moves<context.side_to_move, context.castling_rights>(node.board, king_danger_set, [&](const auto& move) {
    visitor.template visit_king_move<context.move(Node::Context::Move::KING_MOVE)>(move);
  });
}
}  // namespace prodigy::move_generator
