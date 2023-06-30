export module prodigy.move_generator:node;

import prodigy.core;

export namespace prodigy::move_generator {
struct Node {
  struct Context {
    consteval Context pawn_double_push() const noexcept {
      return {
          .side_to_move = !side_to_move,
          .castling_rights = castling_rights,
          .has_en_passant_target = true,
      };
    }

    consteval Context kingside_rook_move(const CastlingRights castling_rights) const noexcept {
      return move([&] {
        switch (side_to_move) {
          case Color::WHITE:
            return castling_rights & ~CastlingRights::WHITE_KINGSIDE;
          case Color::BLACK:
            return castling_rights & ~CastlingRights::BLACK_KINGSIDE;
        }
      }());
    }

    consteval Context queenside_rook_move(const CastlingRights castling_rights) const noexcept {
      return move([&] {
        switch (side_to_move) {
          case Color::WHITE:
            return castling_rights & ~CastlingRights::WHITE_QUEENSIDE;
          case Color::BLACK:
            return castling_rights & ~CastlingRights::BLACK_QUEENSIDE;
        }
      }());
    }

    consteval Context king_move(const CastlingRights castling_rights) const noexcept {
      return move([&] {
        switch (side_to_move) {
          case Color::WHITE:
            return castling_rights & ~(CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE);
          case Color::BLACK:
            return castling_rights & ~(CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE);
        }
      }());
    }

    consteval Context move(const CastlingRights castling_rights) const noexcept {
      return {
          .side_to_move = !side_to_move,
          .castling_rights = castling_rights,
          .has_en_passant_target = false,
      };
    }

    friend consteval bool operator==(Context, Context) = default;

    Color side_to_move;
    CastlingRights castling_rights;
    bool has_en_passant_target;
  };

  friend consteval bool operator==(const Node&, const Node&) = default;

  Board board;
  Bitboard en_passant_target;
};
}  // namespace prodigy::move_generator
