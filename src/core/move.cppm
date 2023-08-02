export module prodigy.core:move;

import :bitboard;
import :piece_type;

export namespace prodigy {
struct QuietMove final {
  Bitboard origin;
  Bitboard target;
  PieceType piece_type;
};

struct Capture final {
  Bitboard origin;
  Bitboard target;
  PieceType aggressor;
  PieceType victim;
};

struct Castle final {
  Bitboard king_origin;
  Bitboard king_target;
  Bitboard rook_origin;
  Bitboard rook_target;
};

struct QuietPromotion final {
  Bitboard origin;
  Bitboard target;
  PieceType promotion;
};

struct CapturePromotion final {
  Bitboard origin;
  Bitboard target;
  PieceType promotion;
  PieceType victim;
};

struct EnPassant final {
  Bitboard origin;
  Bitboard target;
  Bitboard victim_origin;
};
}  // namespace prodigy
