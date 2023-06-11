module;

#include <magic_enum.hpp>
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

  constexpr void move(const QuietMove& move) noexcept {
    const auto& [origin, target, side_to_move, piece_type] = move;
    const auto mask = origin | target;
    toggle(side_to_move, piece_type, mask);
    occupancy_ ^= mask;
  }

  constexpr void move(const Capture& move) noexcept {
    const auto& [origin, target, side_to_move, aggressor, victim] = move;
    toggle(side_to_move, aggressor, origin | target);
    toggle(!side_to_move, victim, target);
    occupancy_ ^= origin;
  }

  constexpr void move(const Castle& move) noexcept {
    const auto& [king_origin, king_target, rook_origin, rook_target, side_to_move] = move;
    const auto king_mask = king_origin | king_target;
    pieces_[side_to_move][PieceType::KING] ^= king_mask;
    const auto rook_mask = rook_origin | rook_target;
    pieces_[side_to_move][PieceType::ROOK] ^= rook_mask;
    const auto color_mask = king_mask | rook_mask;
    colors_[side_to_move] ^= color_mask;
    occupancy_ ^= color_mask;
  }

  constexpr void move(const QuietPromotion& move) noexcept {
    const auto& [origin, target, side_to_move, promotion] = move;
    pieces_[side_to_move][PieceType::PAWN] ^= origin;
    pieces_[side_to_move][promotion] ^= target;
    const auto color_mask = origin | target;
    colors_[side_to_move] ^= color_mask;
    occupancy_ ^= color_mask;
  }

  constexpr void move(const CapturePromotion& move) noexcept {
    const auto& [origin, target, side_to_move, promotion, victim] = move;
    pieces_[side_to_move][PieceType::PAWN] ^= origin;
    pieces_[side_to_move][promotion] ^= target;
    colors_[side_to_move] ^= origin | target;
    toggle(!side_to_move, victim, target);
    occupancy_ ^= origin;
  }

  constexpr void move(const EnPassantCapture& move) noexcept {
    const auto& [origin, target, victim_origin, side_to_move] = move;
    this->move({
        .origin = origin,
        .target = target,
        .side_to_move = side_to_move,
        .piece_type = PieceType::PAWN,
    });
    toggle(!side_to_move, PieceType::PAWN, victim_origin);
    occupancy_ ^= victim_origin;
  }

  friend constexpr bool operator==(const Board&, const Board&) = default;

 private:
  constexpr void toggle(const Color color, const PieceType piece_type, const Bitboard mask) noexcept {
    pieces_[color][piece_type] ^= mask;
    colors_[color] ^= mask;
  }

  EnumMap<Color, EnumMap<PieceType, Bitboard>> pieces_{};
  EnumMap<Color, Bitboard> colors_{};
  Bitboard occupancy_{};
};
}  // namespace prodigy
