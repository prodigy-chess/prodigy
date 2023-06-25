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
constexpr void walk_castle(const Board& board, const Bitboard king_danger_set, const auto& visit_move) {
  if constexpr (using CastlingTraits = ColorTraits<side_to_move>::template CastlingTraits<side>;
                any(castling_rights & CastlingTraits::CASTLING_RIGHTS)) {
    static constexpr auto& castle = CastlingTraits::CASTLE;
    if (static constexpr auto rook_path =
            half_open_segment(square_of(castle.rook_target), square_of(castle.rook_origin));
        empty(board.occupancy() & rook_path) &&
        empty(king_danger_set & (castle.king_origin | castle.rook_target | castle.king_target))) {
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
  walk_castle<side_to_move, castling_rights, PieceType::KING>(board, king_danger_set, visit_move);
  walk_castle<side_to_move, castling_rights, PieceType::QUEEN>(board, king_danger_set, visit_move);
}

template <Color side_to_move>
Bitboard make_checkers(const Board& board, const Square king_origin) noexcept {
  // FIXME: try short circuiting using empty(king_danger_set & board[side_to_move, PieceType::KING])
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
  const auto pinners = lookup_attack_set(king_origin, board.occupancy() & ~attack_set) &
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

template <Color side_to_move>
constexpr void walk_pawn_pushes(const Board& board, const Bitboard check_mask, const Bitboard dg_pin_mask,
                                const Bitboard hv_pin_mask, const auto& visit_single_push,
                                const std::invocable<const QuietMove&, Bitboard> auto& visit_double_push) {
  const auto walk_pushes = [&, empty = ~board.occupancy()](const auto origins, const auto target_mask) {
    const auto unmasked_single_push_targets = pawn_single_push_set(side_to_move, origins) & empty;
    const auto single_push_targets = unmasked_single_push_targets & target_mask;
    const auto promotion_targets = single_push_targets & ColorTraits<side_to_move>::PROMOTION_RANK;
    for_each_bit(promotion_targets, [&](const auto target) {
      for (const auto promotion : {PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
        visit_single_push(QuietPromotion{
            .origin = pawn_single_push_origin(side_to_move, target),
            .target = target,
            .side_to_move = side_to_move,
            .promotion = promotion,
        });
      }
    });
    for_each_bit(single_push_targets ^ promotion_targets, [&](const auto target) {
      visit_single_push(QuietMove{
          .origin = pawn_single_push_origin(side_to_move, target),
          .target = target,
          .side_to_move = side_to_move,
          .piece_type = PieceType::PAWN,
      });
    });
    const auto double_push_targets =
        pawn_single_push_set(side_to_move,
                             unmasked_single_push_targets & ColorTraits<side_to_move>::EN_PASSANT_TARGET_RANK) &
        empty & target_mask;
    for_each_bit(double_push_targets, [&](const auto target) {
      const auto en_passant_target = pawn_single_push_origin(side_to_move, target);
      visit_double_push(
          QuietMove{
              .origin = pawn_single_push_origin(side_to_move, en_passant_target),
              .target = target,
              .side_to_move = side_to_move,
              .piece_type = PieceType::PAWN,
          },
          en_passant_target);
    });
  };
  const auto [pinned, unpinned] =
      make_pinned_and_unpinned<side_to_move, PieceType::PAWN>(board, ~dg_pin_mask, hv_pin_mask);
  walk_pushes(unpinned, check_mask);
  // FIXME: constexpr this
  if (check_mask == ~Bitboard()) {
    walk_pushes(pinned, hv_pin_mask);
  }
}

template <Color side_to_move>
void walk_en_passant_captures(const Node& node, const Bitboard check_mask, const Bitboard dg_pin_mask,
                              const Bitboard dg_pinned, const Bitboard unpinned,
                              const std::invocable<const EnPassantCapture&> auto& visit_move) {
  const auto walk_single_direction_en_passant_capture = [&](const auto origins, const auto& lookup_attack_set) {
    if (const auto origin = origins & lookup_attack_set(!side_to_move, node.en_passant_target); any(origin)) {
      // FIXME: optimize (only compute once?, pass in king_origin?)
      if (const auto victim_origin = pawn_single_push_set(!side_to_move, node.en_passant_target);
          empty(rook_attack_set(square_of(node.board[side_to_move, PieceType::KING]),
                                node.board.occupancy() ^ origin ^ node.en_passant_target ^ victim_origin) &
                (node.board[!side_to_move, PieceType::ROOK] | node.board[!side_to_move, PieceType::QUEEN]))) {
        visit_move(EnPassantCapture{
            .origin = origin,
            .target = node.en_passant_target,
            .victim_origin = victim_origin,
            .side_to_move = side_to_move,
        });
      }
    }
  };
  const auto walk_en_passant_captures = [&](const auto origins) {
    walk_single_direction_en_passant_capture(origins, pawn_left_attack_set);
    walk_single_direction_en_passant_capture(origins, pawn_right_attack_set);
  };
  walk_en_passant_captures(unpinned);
  // FIXME: constexpr this
  if (check_mask == ~Bitboard()) {
    if (any(dg_pin_mask & node.en_passant_target)) {
      walk_en_passant_captures(dg_pinned);
    }
  }
}

template <Color side_to_move, bool has_en_passant_target>
constexpr void walk_pawn_captures(const Node& node, const Bitboard check_mask, const Bitboard dg_pin_mask,
                                  const Bitboard hv_pin_mask, const auto& visit_move) {
  const auto walk_single_direction_captures = [&](const auto origins, const auto target_mask,
                                                  const auto& lookup_attack_set, const auto& origin_of) {
    const auto targets = lookup_attack_set(side_to_move, origins) & node.board[!side_to_move] & target_mask;
    const auto promotion_targets = targets & ColorTraits<side_to_move>::PROMOTION_RANK;
    for_each_bit(promotion_targets, [&](const auto target) {
      for (const auto promotion : {PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
        visit_move(CapturePromotion{
            .origin = origin_of(side_to_move, target),
            .target = target,
            .side_to_move = side_to_move,
            .promotion = promotion,
            .victim = capture_victim_at<side_to_move>(node.board, target),
        });
      }
    });
    for_each_bit(targets ^ promotion_targets, [&](const auto target) {
      visit_move(Capture{
          .origin = origin_of(side_to_move, target),
          .target = target,
          .side_to_move = side_to_move,
          .aggressor = PieceType::PAWN,
          .victim = capture_victim_at<side_to_move>(node.board, target),
      });
    });
  };
  const auto walk_captures = [&](const auto origins, const auto target_mask) {
    walk_single_direction_captures(origins, target_mask, pawn_left_attack_set, pawn_left_capture_origin);
    walk_single_direction_captures(origins, target_mask, pawn_right_attack_set, pawn_right_capture_origin);
  };
  const auto [pinned, unpinned] =
      make_pinned_and_unpinned<side_to_move, PieceType::PAWN>(node.board, ~hv_pin_mask, dg_pin_mask);
  walk_captures(unpinned, check_mask);
  // FIXME: constexpr this
  if (check_mask == ~Bitboard()) {
    walk_captures(pinned, dg_pin_mask);
  }
  if constexpr (has_en_passant_target) {
    walk_en_passant_captures<side_to_move>(node, check_mask, dg_pin_mask, pinned, unpinned, visit_move);
  }
}

template <Color side_to_move, bool has_en_passant_target>
void walk_pawn_moves(const Node& node, const Bitboard check_mask, const Bitboard dg_pin_mask,
                     const Bitboard hv_pin_mask, const auto& visit_single_push_or_capture,
                     const std::invocable<const QuietMove&, Bitboard> auto& visit_double_push) {
  walk_pawn_pushes<side_to_move>(node.board, check_mask, dg_pin_mask, hv_pin_mask, visit_single_push_or_capture,
                                 visit_double_push);
  walk_pawn_captures<side_to_move, has_en_passant_target>(node, check_mask, dg_pin_mask, hv_pin_mask,
                                                          visit_single_push_or_capture);
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
  walk_pawn_moves<context.side_to_move, context.has_en_passant_target>(
      node, check_mask, dg_pin_mask, hv_pin_mask,
      [&](const auto& move) { visitor.template visit_pawn_move<context.move()>(move); },
      [&](const QuietMove& double_push, const Bitboard en_passant_target) {
        visitor.template visit_pawn_move<context.move(Node::Context::Move::PAWN_DOUBLE_PUSH)>(double_push,
                                                                                              en_passant_target);
      });
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
