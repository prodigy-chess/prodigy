export module prodigy.move_generator:node;

import prodigy.core;

export namespace prodigy::move_generator {
struct Node {
  struct Context {
    enum class Move { PAWN_DOUBLE_PUSH, KINGSIDE_ROOK_MOVE, QUEENSIDE_ROOK_MOVE, KING_MOVE, OTHER };

    consteval Context move(const Move move = Move::OTHER) const noexcept {
      return {
          .side_to_move = !side_to_move,
          .castling_rights =
              [&] {
                switch (move) {
                  case Move::KINGSIDE_ROOK_MOVE:
                    switch (side_to_move) {
                      case Color::WHITE:
                        return castling_rights & ~CastlingRights::WHITE_KINGSIDE;
                      case Color::BLACK:
                        return castling_rights & ~CastlingRights::BLACK_KINGSIDE;
                    }
                  case Move::QUEENSIDE_ROOK_MOVE:
                    switch (side_to_move) {
                      case Color::WHITE:
                        return castling_rights & ~CastlingRights::WHITE_QUEENSIDE;
                      case Color::BLACK:
                        return castling_rights & ~CastlingRights::BLACK_QUEENSIDE;
                    }
                  case Move::KING_MOVE:
                    switch (side_to_move) {
                      case Color::WHITE:
                        return castling_rights & ~(CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE);
                      case Color::BLACK:
                        return castling_rights & ~(CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE);
                    }
                  case Move::PAWN_DOUBLE_PUSH:
                  case Move::OTHER:
                    return castling_rights;
                }
              }(),
          .has_en_passant_target = move == Move::PAWN_DOUBLE_PUSH,
      };
    }

    friend consteval bool operator==(Context, Context) = default;

    Color side_to_move;
    CastlingRights castling_rights;
    bool has_en_passant_target;
  };

  Board board;
  Bitboard en_passant_target;
};
}  // namespace prodigy::move_generator
