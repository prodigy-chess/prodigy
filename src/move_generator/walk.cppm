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
constexpr void for_each_capture(const Board& board, const Bitboard targets,
                                const std::invocable<Bitboard, PieceType> auto& callback) {
  for_each_bit(targets, [&](const auto target) {
    for (const auto piece_type : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK}) {
      if (any(board[!side_to_move, piece_type] & target)) {
        callback(target, piece_type);
        return;
      }
    }
    callback(target, PieceType::QUEEN);
  });
}

template <Color side_to_move, CastlingRights castling_rights>
constexpr void for_each_capture(const Board& board, Bitboard targets, const auto& callback) {
#define _(SIDE)                                                                                                   \
  if constexpr (using CastlingTraits = ColorTraits<!side_to_move>::template CastlingTraits<SIDE>;                 \
                any(castling_rights & CastlingTraits::CASTLING_RIGHTS)) {                                         \
    if (const auto target = board[!side_to_move, PieceType::ROOK] & targets & CastlingTraits::CASTLE.rook_origin; \
        any(target)) {                                                                                            \
      callback.template operator()<castling_rights & ~CastlingTraits::CASTLING_RIGHTS>(target, PieceType::ROOK);  \
      targets ^= target;                                                                                          \
    }                                                                                                             \
  }
  _(PieceType::KING)
  _(PieceType::QUEEN)
#undef _
  for_each_capture<side_to_move>(board, targets, [&](const auto target, const auto victim) {
    callback.template operator()<castling_rights>(target, victim);
  });
}

template <Color side_to_move, CastlingRights castling_rights, PieceType piece_type>
constexpr void walk_non_pawn_quiet_moves_and_captures(const Board& board, const Bitboard origin,
                                                      const Bitboard attack_set, const auto& visit_move) {
  for_each_bit(attack_set & ~board.occupancy(), [&](const auto target) {
    visit_move.template operator()<castling_rights>(QuietMove{
        .origin = origin,
        .target = target,
        .piece_type = piece_type,
    });
  });
  for_each_capture<side_to_move, castling_rights>(
      board, attack_set & board[!side_to_move], [&]<auto new_castling_rights>(const auto target, const auto victim) {
        visit_move.template operator()<new_castling_rights>(Capture{
            .origin = origin,
            .target = target,
            .aggressor = piece_type,
            .victim = victim,
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
                           pawn_right_attack_set(!side_to_move, board[!side_to_move, PieceType::PAWN]) |
                           king_attack_set(square_of(board[!side_to_move, PieceType::KING]));
    for_each_square(board[!side_to_move, PieceType::KNIGHT],
                    [&](const auto origin) { king_danger_set |= knight_attack_set(origin); });
    const auto kingless_occupancy = board.occupancy() ^ board[side_to_move, PieceType::KING];
    for_each_square(board[!side_to_move, PieceType::BISHOP] | board[!side_to_move, PieceType::QUEEN],
                    [&](const auto origin) { king_danger_set |= bishop_attack_set(origin, kingless_occupancy); });
    for_each_square(board[!side_to_move, PieceType::ROOK] | board[!side_to_move, PieceType::QUEEN],
                    [&](const auto origin) { king_danger_set |= rook_attack_set(origin, kingless_occupancy); });
    return king_danger_set;
  }();
  walk_non_pawn_quiet_moves_and_captures<side_to_move, castling_rights, PieceType::KING>(
      board, board[side_to_move, PieceType::KING], king_attack_set(king_origin) & ~king_danger_set, visit_move);
#define _(SIDE)                                     \
  walk_castle<side_to_move, castling_rights, SIDE>( \
      board, king_danger_set, [&](const auto& move) { visit_move.template operator()<castling_rights>(move); })
  _(PieceType::KING);
  _(PieceType::QUEEN);
#undef _
}

template <Color side_to_move>
Bitboard make_checkers(const Board& board, const Square king_origin) noexcept {
  auto checkers = (pawn_left_attack_set(side_to_move, board[side_to_move, PieceType::KING]) |
                   pawn_right_attack_set(side_to_move, board[side_to_move, PieceType::KING])) &
                  board[!side_to_move, PieceType::PAWN];
  checkers |= knight_attack_set(king_origin) & board[!side_to_move, PieceType::KNIGHT];
  checkers |= bishop_attack_set(king_origin, board.occupancy()) &
              (board[!side_to_move, PieceType::BISHOP] | board[!side_to_move, PieceType::QUEEN]);
  checkers |= rook_attack_set(king_origin, board.occupancy()) &
              (board[!side_to_move, PieceType::ROOK] | board[!side_to_move, PieceType::QUEEN]);
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
constexpr void walk_pawn_pushes(const Board& board, const Bitboard dg_pin_mask, const Bitboard hv_pin_mask,
                                const auto& visit_single_push,
                                const std::invocable<const QuietMove&> auto& visit_double_push,
                                const std::same_as<Bitboard> auto... check_mask) {
  const auto walk_pushes = [&, empty = ~board.occupancy()](const auto origins, const auto... target_mask) {
    const auto unmasked_single_push_targets = pawn_single_push_set(side_to_move, origins) & empty;
    const auto single_push_targets = (unmasked_single_push_targets & ... & target_mask);
    const auto promotion_targets = single_push_targets & ColorTraits<side_to_move>::PROMOTION_RANK;
    for_each_bit(promotion_targets, [&](const auto target) {
      for (const auto promotion : {PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
        visit_single_push(QuietPromotion{
            .origin = pawn_single_push_origin(side_to_move, target),
            .target = target,
            .promotion = promotion,
        });
      }
    });
    for_each_bit(single_push_targets ^ promotion_targets, [&](const auto target) {
      visit_single_push(QuietMove{
          .origin = pawn_single_push_origin(side_to_move, target),
          .target = target,
          .piece_type = PieceType::PAWN,
      });
    });
    const auto double_push_targets =
        pawn_single_push_set(side_to_move,
                             unmasked_single_push_targets & ColorTraits<side_to_move>::EN_PASSANT_TARGET_RANK) &
        (empty & ... & target_mask);
    for_each_bit(double_push_targets, [&](const auto target) {
      visit_double_push(QuietMove{
          .origin = pawn_single_push_origin(side_to_move, pawn_single_push_origin(side_to_move, target)),
          .target = target,
          .piece_type = PieceType::PAWN,
      });
    });
  };
  const auto [pinned, unpinned] =
      make_pinned_and_unpinned<side_to_move, PieceType::PAWN>(board, ~dg_pin_mask, hv_pin_mask);
  walk_pushes(unpinned, check_mask...);
  if constexpr (sizeof...(check_mask) == 0) {
    walk_pushes(pinned, hv_pin_mask);
  }
}

template <Color side_to_move>
void walk_en_passants(const Node& node, const Bitboard dg_pin_mask, const Bitboard dg_pinned, const Bitboard unpinned,
                      const std::invocable<const EnPassant&> auto& visit_move,
                      const std::same_as<Bitboard> auto... check_mask) {
  const auto target = pawn_single_push_set(side_to_move, node.en_passant_victim_origin);
  const auto walk_en_passants = [&](const auto origins) {
    for (const auto direction : {Direction::EAST, Direction::WEST}) {
      if (const auto origin = origins & shift(node.en_passant_victim_origin, direction);
          any(origin) &&
          empty(rook_attack_set(square_of(node.board[side_to_move, PieceType::KING]),
                                node.board.occupancy() ^ target ^ node.en_passant_victim_origin ^ origin) &
                (node.board[!side_to_move, PieceType::ROOK] | node.board[!side_to_move, PieceType::QUEEN]))) {
        visit_move(EnPassant{
            .origin = origin,
            .target = target,
            .victim_origin = node.en_passant_victim_origin,
        });
      }
    }
  };
  if constexpr (sizeof...(check_mask) == 0) {
    walk_en_passants(unpinned);
    if (any(dg_pin_mask & target)) {
      walk_en_passants(dg_pinned);
    }
  } else if (any(((target | node.en_passant_victim_origin) & ... & check_mask))) {
    walk_en_passants(unpinned);
  }
}

template <Node::Context context>
constexpr void walk_pawn_captures(const Node& node, const Bitboard dg_pin_mask, const Bitboard hv_pin_mask,
                                  const auto& visit_move, const std::same_as<Bitboard> auto... check_mask) {
  const auto walk_single_direction_captures = [&](const auto origins, const auto& lookup_attack_set,
                                                  const auto& origin_of, const auto... target_mask) {
    const auto targets =
        lookup_attack_set(context.side_to_move, origins) & (node.board[!context.side_to_move] & ... & target_mask);
    const auto promotion_targets = targets & ColorTraits<context.side_to_move>::PROMOTION_RANK;
    for_each_capture<context.side_to_move, context.castling_rights>(
        node.board, promotion_targets, [&]<auto new_castling_rights>(const auto target, const auto victim) {
          for (const auto promotion : {PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
            visit_move.template operator()<new_castling_rights>(CapturePromotion{
                .origin = origin_of(context.side_to_move, target),
                .target = target,
                .promotion = promotion,
                .victim = victim,
            });
          }
        });
    for_each_capture<context.side_to_move>(node.board, targets ^ promotion_targets,
                                           [&](const auto target, const auto victim) {
                                             visit_move.template operator()<context.castling_rights>(Capture{
                                                 .origin = origin_of(context.side_to_move, target),
                                                 .target = target,
                                                 .aggressor = PieceType::PAWN,
                                                 .victim = victim,
                                             });
                                           });
  };
  const auto walk_captures = [&](const auto origins, const auto... target_mask) {
    walk_single_direction_captures(origins, pawn_left_attack_set, pawn_left_capture_origin, target_mask...);
    walk_single_direction_captures(origins, pawn_right_attack_set, pawn_right_capture_origin, target_mask...);
  };
  const auto [pinned, unpinned] =
      make_pinned_and_unpinned<context.side_to_move, PieceType::PAWN>(node.board, ~hv_pin_mask, dg_pin_mask);
  walk_captures(unpinned, check_mask...);
  if constexpr (sizeof...(check_mask) == 0) {
    walk_captures(pinned, dg_pin_mask);
  }
  if constexpr (context.can_en_passant) {
    walk_en_passants<context.side_to_move>(
        node, dg_pin_mask, pinned, unpinned,
        [&](const auto& move) { visit_move.template operator()<context.castling_rights>(move); }, check_mask...);
  }
}

template <Node::Context context>
void walk_pawn_moves(const Node& node, const Bitboard dg_pin_mask, const Bitboard hv_pin_mask,
                     const auto& visit_single_push_or_capture,
                     const std::invocable<const QuietMove&> auto& visit_double_push,
                     const std::same_as<Bitboard> auto... check_mask) {
  walk_pawn_pushes<context.side_to_move>(
      node.board, dg_pin_mask, hv_pin_mask,
      [&](const auto& move) { visit_single_push_or_capture.template operator()<context.castling_rights>(move); },
      visit_double_push, check_mask...);
  walk_pawn_captures<context>(node, dg_pin_mask, hv_pin_mask, visit_single_push_or_capture, check_mask...);
}

template <Color side_to_move, CastlingRights castling_rights, PieceType piece_type>
void walk_piece_moves(const Board& board, const Bitboard origin_mask, const Bitboard pin_mask,
                      const std::invocable<Square> auto& lookup_attack_set, const auto& visit_move,
                      const std::same_as<Bitboard> auto... check_mask) {
  const auto walk_moves = [&](const auto origins, const auto... target_mask) {
    for_each_bit_and_square(origins, [&](const auto origin_bit, const auto origin_square) {
      walk_non_pawn_quiet_moves_and_captures<side_to_move, castling_rights, piece_type>(
          board, origin_bit, (lookup_attack_set(origin_square) & ... & target_mask), visit_move);
    });
  };
  const auto [pinned, unpinned] = make_pinned_and_unpinned<side_to_move, piece_type>(board, origin_mask, pin_mask);
  walk_moves(unpinned, check_mask...);
  if constexpr (piece_type != PieceType::KNIGHT && sizeof...(check_mask) == 0) {
    walk_moves(pinned, pin_mask);
  }
}

template <Node::Context context, typename T>
void walk_non_king_moves(const Node& node, const Square king_origin, Visitor<T>& visitor,
                         const std::same_as<Bitboard> auto... check_mask) {
  const auto dg_pin_mask =
      make_pin_mask<context.side_to_move, PieceType::BISHOP>(node.board, king_origin, bishop_attack_set);
  const auto hv_pin_mask =
      make_pin_mask<context.side_to_move, PieceType::ROOK>(node.board, king_origin, rook_attack_set);
  walk_pawn_moves<context>(
      node, dg_pin_mask, hv_pin_mask,
      [&]<auto new_castling_rights>(const auto& move) {
        visitor.template visit_pawn_move<context.move(new_castling_rights)>(move);
      },
      [&](const QuietMove& double_push) {
        any(node.board[!context.side_to_move, PieceType::PAWN] &
            (shift(double_push.target, Direction::EAST) | shift(double_push.target, Direction::WEST)))
            ? visitor.template visit_pawn_move<context.enable_en_passant()>(double_push)
            : visitor.template visit_pawn_move<context.move(context.castling_rights)>(double_push);
      },
      check_mask...);
  walk_piece_moves<context.side_to_move, context.castling_rights, PieceType::KNIGHT>(
      node.board, ~(dg_pin_mask | hv_pin_mask), Bitboard(), knight_attack_set,
      [&]<auto new_castling_rights>(const auto& move) {
        visitor.template visit_knight_move<context.move(new_castling_rights)>(move);
      },
      check_mask...);
  walk_piece_moves<context.side_to_move, context.castling_rights, PieceType::BISHOP>(
      node.board, ~hv_pin_mask, dg_pin_mask,
      [&](const auto origin) { return bishop_attack_set(origin, node.board.occupancy()); },
      [&]<auto new_castling_rights>(const auto& move) {
        visitor.template visit_bishop_move<context.move(new_castling_rights)>(move);
      },
      check_mask...);
  walk_piece_moves<context.side_to_move, context.castling_rights, PieceType::ROOK>(
      node.board, ~dg_pin_mask, hv_pin_mask,
      [&](const auto origin) { return rook_attack_set(origin, node.board.occupancy()); },
      [&]<auto new_castling_rights>(const auto& move) {
        switch (using ColorTraits = ColorTraits<context.side_to_move>; move.origin) {
          case ColorTraits::template CastlingTraits<PieceType::KING>::CASTLE.rook_origin:
            visitor.template visit_rook_move<context.move_kingside_rook(new_castling_rights)>(move);
            break;
          case ColorTraits::template CastlingTraits<PieceType::QUEEN>::CASTLE.rook_origin:
            visitor.template visit_rook_move<context.move_queenside_rook(new_castling_rights)>(move);
            break;
          default:
            visitor.template visit_rook_move<context.move(new_castling_rights)>(move);
            break;
        }
      },
      check_mask...);
  const auto walk_queen_moves = [&](const auto origin_mask, const auto pin_mask, const auto& lookup_attack_set) {
    walk_piece_moves<context.side_to_move, context.castling_rights, PieceType::QUEEN>(
        node.board, origin_mask, pin_mask,
        [&](const auto origin) { return lookup_attack_set(origin, node.board.occupancy()); },
        [&]<auto new_castling_rights>(const auto& move) {
          visitor.template visit_queen_move<context.move(new_castling_rights)>(move);
        },
        check_mask...);
  };
  walk_queen_moves(~hv_pin_mask, dg_pin_mask, bishop_attack_set);
  walk_queen_moves(~dg_pin_mask, hv_pin_mask, rook_attack_set);
}
}  // namespace prodigy::move_generator

export namespace prodigy::move_generator {
template <Node::Context context, typename T>
void walk(const Node& node, Visitor<T>&& visitor) {
  const auto king_origin = square_of(node.board[context.side_to_move, PieceType::KING]);
  walk_king_moves<context.side_to_move, context.castling_rights>(
      node.board, king_origin, [&]<auto new_castling_rights>(const auto& move) {
        visitor.template visit_king_move<context.move_king(new_castling_rights)>(move);
      });
  switch (const auto checkers = make_checkers<context.side_to_move>(node.board, king_origin); popcount(checkers)) {
    case 0:
      walk_non_king_moves<context>(node, king_origin, visitor);
      break;
    case 1:
      walk_non_king_moves<context>(node, king_origin, visitor, half_open_segment(square_of(checkers), king_origin));
      break;
    default:
      break;
  }
}
}  // namespace prodigy::move_generator
