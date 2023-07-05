export module prodigy.core:move;

import :bitboard;
import :color;
import :piece_type;

export namespace prodigy {
struct QuietMove final {
  Bitboard origin;
  Bitboard target;
  Color side_to_move;
  PieceType piece_type;
};

struct Capture final {
  Bitboard origin;
  Bitboard target;
  Color side_to_move;
  PieceType aggressor;
  PieceType victim;
};

struct Castle final {
  Bitboard king_origin;
  Bitboard king_target;
  Bitboard rook_origin;
  Bitboard rook_target;
  Color side_to_move;
};

struct QuietPromotion final {
  Bitboard origin;
  Bitboard target;
  Color side_to_move;
  PieceType promotion;
};

struct CapturePromotion final {
  Bitboard origin;
  Bitboard target;
  Color side_to_move;
  PieceType promotion;
  PieceType victim;
};

struct EnPassant final {
  Bitboard origin;
  Bitboard target;
  Bitboard victim_origin;
  Color side_to_move;
};
}  // namespace prodigy
