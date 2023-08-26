export module prodigy.move_generator:node;

import prodigy.core;

export namespace prodigy::move_generator {
struct Node {
  struct Context {
    Color side_to_move;
    CastlingRights castling_rights;
    bool can_en_passant;

    consteval Context enable_en_passant() const noexcept {
      return {
          .side_to_move = !side_to_move,
          .castling_rights = castling_rights,
          .can_en_passant = true,
      };
    }

    consteval Context move(const CastlingRights child_castling_rights) const noexcept {
      return {
          .side_to_move = !side_to_move,
          .castling_rights = child_castling_rights,
          .can_en_passant = false,
      };
    }

    friend consteval bool operator==(Context, Context) = default;
  };

  Board board;
  Bitboard en_passant_victim_origin;

  friend consteval bool operator==(const Node&, const Node&) = default;
};
}  // namespace prodigy::move_generator
