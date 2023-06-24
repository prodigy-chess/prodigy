module;

#include <concepts>
#include <initializer_list>
#include <utility>

export module prodigy.move_generator:walk;

import prodigy.core;

import :lookup;
import :node;
import :visitor;

namespace prodigy::move_generator {
template <Color side_to_move>
constexpr PieceType capture_victim_at(const Board& board, const Bitboard target) noexcept {
  for (const auto piece_type : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK}) {
    if (any(board[!side_to_move, piece_type] & target)) {
      return piece_type;
    }
  }
  return PieceType::QUEEN;
}

template <Color side_to_move, PieceType piece_type>
constexpr void walk_non_pawn_quiet_moves_and_captures(const Board& board, const Bitboard origin,
                                                      const Bitboard attack_set, const auto& visit_move) {
  for_each_bit(attack_set & ~board.occupancy(), [&](const auto target) {
    visit_move(QuietMove{
        .origin = origin,
        .target = target,
        .side_to_move = side_to_move,
        .piece_type = piece_type,
    });
  });
  for_each_bit(attack_set & board[!side_to_move], [&](const auto target) {
    visit_move(Capture{
        .origin = origin,
        .target = target,
        .side_to_move = side_to_move,
        .aggressor = piece_type,
        .victim = capture_victim_at<side_to_move>(board, target),
    });
  });
}

template <Color side_to_move, CastlingRights castling_rights, PieceType side>
constexpr void walk_castle(const Bitboard king_danger_set, const auto& visit_move) {
  if constexpr (using CastlingTraits = ColorTraits<side_to_move>::template CastlingTraits<side>;
                any(castling_rights & CastlingTraits::CASTLING_RIGHTS)) {
    if (static constexpr auto& castle = CastlingTraits::CASTLE;
        !any(king_danger_set & (castle.king_origin | castle.rook_target | castle.king_target))) {
      visit_move(castle);
    }
  }
}

template <Color side_to_move, CastlingRights castling_rights>
void walk_king_moves(const Board& board, const Square king_origin, const auto& visit_move) {
  const auto king_danger_set = [&] {
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
  }();
  walk_non_pawn_quiet_moves_and_captures<side_to_move, PieceType::KING>(
      board, board[side_to_move, PieceType::KING], king_attack_set(king_origin) & ~king_danger_set, visit_move);
  walk_castle<side_to_move, castling_rights, PieceType::KING>(king_danger_set, visit_move);
  walk_castle<side_to_move, castling_rights, PieceType::QUEEN>(king_danger_set, visit_move);
}

template <Color side_to_move>
Bitboard make_checkers(const Board& board, const Square king_origin) noexcept {
  // FIXME: try short circuiting using !any(king_danger_set & board[side_to_move, PieceType::KING])
  auto checkers = (pawn_left_attack_set(side_to_move, board[side_to_move, PieceType::KING]) |
                   pawn_right_attack_set(side_to_move, board[side_to_move, PieceType::KING])) &
                  board[!side_to_move, PieceType::PAWN];
  checkers |= knight_attack_set(king_origin) & board[!side_to_move, PieceType::KNIGHT];
  checkers |= bishop_attack_set(king_origin, board.occupancy()) &
              (board[!side_to_move, PieceType::BISHOP] | board[!side_to_move, PieceType::QUEEN]);
  checkers |= rook_attack_set(king_origin, board.occupancy()) &
              (board[!side_to_move, PieceType::ROOK] | board[!side_to_move, PieceType::QUEEN]);
  checkers |= king_attack_set(king_origin) & board[!side_to_move, PieceType::KING];
  return checkers;
}

template <Color side_to_move, PieceType piece_type>
Bitboard make_pin_mask(const Board& board, const Square king_origin,
                       const std::invocable<Square, Bitboard> auto& lookup_attack_set) noexcept {
  const auto attack_set = lookup_attack_set(king_origin, board.occupancy());
  const auto blockers = attack_set & board[side_to_move];
  const auto pinners = (lookup_attack_set(king_origin, board.occupancy() ^ blockers) ^ attack_set) &
                       (board[!side_to_move, piece_type] | board[!side_to_move, PieceType::QUEEN]);
  Bitboard pin_mask{};
  for_each_square(pinners, [&](const auto origin) { pin_mask |= half_open_segment(origin, king_origin); });
  return pin_mask;
}

template <Color side_to_move, PieceType piece_type>
constexpr std::pair<Bitboard, Bitboard> make_pinned_and_unpinned(const Board& board, const Bitboard origin_mask,
                                                                 const Bitboard pin_mask) noexcept {
  const auto origins = board[side_to_move, piece_type] & origin_mask;
  const auto pinned = origins & pin_mask;
  return {pinned, origins ^ pinned};
}

template <Color side_to_move, PieceType piece_type>
void walk_piece_moves(const Board& board, const Bitboard origin_mask, const Bitboard check_mask,
                      const Bitboard pin_mask, const std::invocable<Square> auto& lookup_attack_set,
                      const auto& visit_move) {
  const auto walk_moves = [&](const auto origins, const auto target_mask) {
    for_each_bit_and_square(origins, [&](const auto origin_bit, const auto origin_square) {
      walk_non_pawn_quiet_moves_and_captures<side_to_move, piece_type>(
          board, origin_bit, lookup_attack_set(origin_square) & target_mask, visit_move);
    });
  };
  const auto [pinned, unpinned] = make_pinned_and_unpinned<side_to_move, piece_type>(board, origin_mask, pin_mask);
  walk_moves(unpinned, check_mask);
  if constexpr (piece_type != PieceType::KNIGHT) {
    // FIXME: constexpr this
    if (check_mask == ~Bitboard()) {
      walk_moves(pinned, pin_mask);
    }
  }
}

template <Node::Context context, typename T>
void walk_non_king_moves(const Node& node, const Square king_origin, const Bitboard check_mask, Visitor<T>& visitor) {
  const auto dg_pin_mask =
      make_pin_mask<context.side_to_move, PieceType::BISHOP>(node.board, king_origin, bishop_attack_set);
  const auto hv_pin_mask =
      make_pin_mask<context.side_to_move, PieceType::ROOK>(node.board, king_origin, rook_attack_set);
  walk_piece_moves<context.side_to_move, PieceType::KNIGHT>(
      node.board, ~(dg_pin_mask | hv_pin_mask), check_mask, Bitboard(), knight_attack_set,
      [&](const auto& move) { visitor.template visit_knight_move<context.move()>(move); });
  walk_piece_moves<context.side_to_move, PieceType::BISHOP>(
      node.board, ~hv_pin_mask, check_mask, dg_pin_mask,
      [&](const auto origin) { return bishop_attack_set(origin, node.board.occupancy()); },
      [&](const auto& move) { visitor.template visit_bishop_move<context.move()>(move); });
  walk_piece_moves<context.side_to_move, PieceType::ROOK>(
      node.board, ~dg_pin_mask, check_mask, hv_pin_mask,
      [&](const auto origin) { return rook_attack_set(origin, node.board.occupancy()); },
      [&](const auto& move) {
        switch (using ColorTraits = ColorTraits<context.side_to_move>; move.origin) {
          case ColorTraits::template CastlingTraits<PieceType::KING>::CASTLE.rook_origin:
            visitor.template visit_rook_move<context.move(Node::Context::Move::KINGSIDE_ROOK_MOVE)>(move);
            break;
          case ColorTraits::template CastlingTraits<PieceType::QUEEN>::CASTLE.rook_origin:
            visitor.template visit_rook_move<context.move(Node::Context::Move::QUEENSIDE_ROOK_MOVE)>(move);
            break;
          default:
            visitor.template visit_rook_move<context.move()>(move);
            break;
        }
      });
  const auto walk_queen_moves = [&](const auto origin_mask, const auto pin_mask, const auto& lookup_attack_set) {
    walk_piece_moves<context.side_to_move, PieceType::QUEEN>(
        node.board, origin_mask, check_mask, pin_mask,
        [&](const auto origin) { return lookup_attack_set(origin, node.board.occupancy()); },
        [&](const auto& move) { visitor.template visit_queen_move<context.move()>(move); });
  };
  walk_queen_moves(~hv_pin_mask, dg_pin_mask, bishop_attack_set);
  walk_queen_moves(~dg_pin_mask, hv_pin_mask, rook_attack_set);
}
}  // namespace prodigy::move_generator

export namespace prodigy::move_generator {
template <Node::Context context, typename T>
void walk(const Node& node, Visitor<T>&& visitor) {
  const auto king_origin = square_of(node.board[context.side_to_move, PieceType::KING]);
  walk_king_moves<context.side_to_move, context.castling_rights>(node.board, king_origin, [&](const auto& move) {
    visitor.template visit_king_move<context.move(Node::Context::Move::KING_MOVE)>(move);
  });
  switch (const auto checkers = make_checkers<context.side_to_move>(node.board, king_origin); popcount(checkers)) {
    case 0:
      walk_non_king_moves<context>(node, king_origin, ~Bitboard(), visitor);
      break;
    case 1:
      walk_non_king_moves<context>(node, king_origin, half_open_segment(square_of(checkers), king_origin), visitor);
      break;
    default:
      break;
  }
}
}  // namespace prodigy::move_generator
