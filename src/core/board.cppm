module;

#include <magic_enum/magic_enum_utility.hpp>
#include <optional>
#include <utility>

export module prodigy.core:board;

import :bitboard;
import :color;
import :containers;
import :move;
import :piece_type;
import :square;

export namespace prodigy {
class Board {
 public:
  constexpr Board() = default;

  constexpr explicit Board(
      const EnumMap<Square, std::optional<std::pair<Color, PieceType>>>& piece_placement) noexcept {
    magic_enum::enum_for_each<Square>([&](const auto square) {
      if (const auto piece = piece_placement[square]; piece.has_value()) {
        occupancy_ |= colors_[piece->first] |= pieces_[piece->first][piece->second] |= to_bitboard(square);
      }
    });
  }

  constexpr Bitboard operator[](const Color color, const PieceType piece_type) const noexcept {
    return pieces_[color][piece_type];
  }

  constexpr Bitboard operator[](const Color color) const noexcept { return colors_[color]; }

  constexpr Bitboard occupancy() const noexcept { return occupancy_; }

  template <Color side_to_move>
  constexpr void apply(const QuietMove& move) noexcept {
    const auto& [origin, target, piece_type] = move;
    const auto mask = origin | target;
    toggle<side_to_move>(piece_type, mask);
    occupancy_ ^= mask;
  }

  template <Color side_to_move>
  constexpr void apply(const Capture& move) noexcept {
    const auto& [origin, target, aggressor, victim] = move;
    toggle<side_to_move>(aggressor, origin | target);
    toggle<!side_to_move>(victim, target);
    occupancy_ ^= origin;
  }

  template <Color side_to_move>
  constexpr void apply(const Castle& move) noexcept {
    const auto& [king_origin, king_target, rook_origin, rook_target] = move;
    const auto king_mask = king_origin | king_target;
    pieces_[side_to_move][PieceType::KING] ^= king_mask;
    const auto rook_mask = rook_origin | rook_target;
    pieces_[side_to_move][PieceType::ROOK] ^= rook_mask;
    const auto color_mask = king_mask | rook_mask;
    colors_[side_to_move] ^= color_mask;
    occupancy_ ^= color_mask;
  }

  template <Color side_to_move>
  constexpr void apply(const QuietPromotion& move) noexcept {
    const auto& [origin, target, promotion] = move;
    pieces_[side_to_move][PieceType::PAWN] ^= origin;
    pieces_[side_to_move][promotion] ^= target;
    const auto color_mask = origin | target;
    colors_[side_to_move] ^= color_mask;
    occupancy_ ^= color_mask;
  }

  template <Color side_to_move>
  constexpr void apply(const CapturePromotion& move) noexcept {
    const auto& [origin, target, promotion, victim] = move;
    pieces_[side_to_move][PieceType::PAWN] ^= origin;
    pieces_[side_to_move][promotion] ^= target;
    colors_[side_to_move] ^= origin | target;
    toggle<!side_to_move>(victim, target);
    occupancy_ ^= origin;
  }

  template <Color side_to_move>
  constexpr void apply(const EnPassant& move) noexcept {
    const auto& [origin, target, victim_origin] = move;
    apply<side_to_move>({
        .origin = origin,
        .target = target,
        .piece_type = PieceType::PAWN,
    });
    toggle<!side_to_move>(PieceType::PAWN, victim_origin);
    occupancy_ ^= victim_origin;
  }

  friend constexpr bool operator==(const Board& lhs, const Board& rhs) noexcept { return lhs.pieces_ == rhs.pieces_; }

 private:
  template <Color color>
  constexpr void toggle(const PieceType piece_type, const Bitboard mask) noexcept {
    pieces_[color][piece_type] ^= mask;
    colors_[color] ^= mask;
  }

  EnumMap<Color, EnumMap<PieceType, Bitboard>> pieces_{};
  EnumMap<Color, Bitboard> colors_{};
  Bitboard occupancy_{};
};
}  // namespace prodigy
