module;

#include <algorithm>
#include <cstdint>
#include <functional>
#include <magic_enum_utility.hpp>
#include <type_traits>
#include <utility>

export module prodigy.evaluation:evaluator;

import prodigy.core;

import :phase;
import :piece_values;
import :psqt;

export namespace prodigy::evaluation {
class Evaluator {
 public:
  constexpr void on_search_start(const Board& board) noexcept {
    base_state_ = State();
    magic_enum::enum_for_each<Color>([&](const auto color) {
      magic_enum::enum_for_each<PieceType>([&](const auto piece_type) {
        for_each_square(board[color, piece_type], [&](const auto square) {
          magic_enum::enum_for_each<Phase>(
              [&](const auto phase) { base_state_.terms[phase][color] += TABLES[phase][piece_type][color][square]; });
          base_state_.phase += PHASE_DELTAS[piece_type];
        });
      });
    });
  }

  constexpr void on_simulation_start() noexcept { active_state_ = base_state_; }

  template <Color side_to_move>
  constexpr void on_move(const QuietMove& move) noexcept {
    update_square<side_to_move>(move.piece_type, square_of(move.origin), square_of(move.target));
  }

  template <Color side_to_move>
  constexpr void on_move(const Capture& move) noexcept {
    const auto target = square_of(move.target);
    update_square<side_to_move>(move.aggressor, square_of(move.origin), target);
    remove<!side_to_move>(move.victim, target);
  }

  template <Color side_to_move>
  constexpr void on_move(const Castle& move) noexcept {
    update_square<side_to_move>(PieceType::KING, square_of(move.king_origin), square_of(move.king_target));
    update_square<side_to_move>(PieceType::KING, square_of(move.rook_origin), square_of(move.rook_target));
  }

  template <Color side_to_move>
  constexpr void on_move(const QuietPromotion& move) noexcept {
    remove<side_to_move>(PieceType::PAWN, square_of(move.origin));
    add<side_to_move>(move.promotion, square_of(move.target));
  }

  template <Color side_to_move>
  constexpr void on_move(const CapturePromotion& move) noexcept {
    const auto target = square_of(move.target);
    remove<side_to_move>(PieceType::PAWN, square_of(move.origin));
    remove<!side_to_move>(move.victim, target);
    add<side_to_move>(move.promotion, target);
  }

  template <Color side_to_move>
  constexpr void on_move(const EnPassant& move) noexcept {
    update_square<side_to_move>(PieceType::PAWN, square_of(move.origin), square_of(move.target));
    remove<!side_to_move>(PieceType::PAWN, square_of(move.victim_origin));
  }

  template <Color side_to_move>
  constexpr float evaluate() const noexcept {
    static constexpr Value MAX_PHASE = 24;
    EnumMap<Phase, Value> evaluations;
    magic_enum::enum_for_each<Phase>([&](const auto phase) {
      evaluations[phase] = active_state_.terms[phase][side_to_move] - active_state_.terms[phase][!side_to_move];
    });
    const auto midgame_phase = std::min(active_state_.phase, MAX_PHASE);
    const auto endgame_phase = MAX_PHASE - midgame_phase;
    return static_cast<float>(evaluations[Phase::MIDGAME] * midgame_phase +
                              evaluations[Phase::ENDGAME] * endgame_phase) /
           MAX_PHASE;
  }

 private:
  using Value = std::int_fast32_t;

  struct State {
    EnumMap<Phase, EnumMap<Color, Value>> terms;
    Value phase;
  };

  template <Color color>
  constexpr void for_each_term(const PieceType piece_type, auto&& callback) noexcept {
    magic_enum::enum_for_each<Phase>([&](const auto phase) {
      std::invoke(std::forward<decltype(callback)>(callback), active_state_.terms[phase][color],
                  TABLES[phase][piece_type][color]);
    });
  }

  template <Color color>
  constexpr void update_square(const PieceType piece_type, const Square origin, const Square target) noexcept {
    for_each_term<color>(piece_type, [&](auto& term, const auto& table) { term += table[target] - table[origin]; });
  }

  template <Color color>
  constexpr void add(const PieceType piece_type, const Square origin) noexcept {
    for_each_term<color>(piece_type, [&](auto& term, const auto& table) { term += table[origin]; });
    active_state_.phase += PHASE_DELTAS[piece_type];
  }

  template <Color color>
  constexpr void remove(const PieceType piece_type, const Square origin) noexcept {
    for_each_term<color>(piece_type, [&](auto& term, const auto& table) { term -= table[origin]; });
    active_state_.phase -= PHASE_DELTAS[piece_type];
  }

  static constexpr auto TABLES = [] {
    std::remove_const_t<decltype(PSQT)> tables;
    magic_enum::enum_for_each<Phase>([&](const auto phase) {
      magic_enum::enum_for_each<PieceType>([&](const auto piece_type) {
        magic_enum::enum_for_each<Color>([&](const auto color) {
          magic_enum::enum_for_each<Square>([&](const auto square) {
            tables[phase][piece_type][color][square] =
                PSQT[phase][piece_type][color][square] + PIECE_VALUES[phase][piece_type];
          });
        });
      });
    });
    return tables;
  }();

  State base_state_;
  State active_state_;
};
}  // namespace prodigy::evaluation
