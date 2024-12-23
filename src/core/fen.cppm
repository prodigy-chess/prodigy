module;

#include <charconv>
#include <expected>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>

export module prodigy.core:fen;

import :bitboard;
import :board;
import :castling_rights;
import :color;
import :containers;
import :direction;
import :piece_type;
import :ply;
import :position;
import :square;

export namespace prodigy {
inline constexpr auto STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
inline constexpr auto KIWIPETE = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

[[nodiscard]] constexpr std::expected<Position, std::string_view> parse_fen(const std::string_view fen) noexcept {
  Position position{};
  auto field_index = 0UZ;
  for (const auto field : fen | std::views::split(' ') | std::views::transform([](auto&& range) {
                            return std::string_view(std::forward<decltype(range)>(range));
                          })) {
    const auto from_chars = [&](auto& value) {
      return std::from_chars(field.begin(), field.end(), value) == std::from_chars_result{field.end(), std::errc()};
    };
    switch (field_index++) {
      case 0: {
        EnumMap<Square, std::optional<std::pair<Color, PieceType>>> piece_placement;
        auto rank_index = magic_enum::enum_count<Rank>() - 1UZ;
        for (const auto rank_contents : field | std::views::split('/')) {
          if (rank_index >= magic_enum::enum_count<Rank>()) {
            return std::unexpected("Too many ranks.");
          }
          auto file_index = 0UZ;
          for (const auto rank = magic_enum::enum_value<Rank>(rank_index--); const auto file_contents : rank_contents) {
            if (file_index >= magic_enum::enum_count<File>()) {
              return std::unexpected("Too many files.");
            }
            switch (auto& piece = piece_placement[to_square(magic_enum::enum_value<File>(file_index), rank)];
                    file_contents) {
#define _(CHAR, COLOR, PIECE_TYPE)    \
  case CHAR:                          \
    piece.emplace(COLOR, PIECE_TYPE); \
    ++file_index;                     \
    break
              _('P', Color::WHITE, PieceType::PAWN);
              _('N', Color::WHITE, PieceType::KNIGHT);
              _('B', Color::WHITE, PieceType::BISHOP);
              _('R', Color::WHITE, PieceType::ROOK);
              _('Q', Color::WHITE, PieceType::QUEEN);
              _('K', Color::WHITE, PieceType::KING);
              _('p', Color::BLACK, PieceType::PAWN);
              _('n', Color::BLACK, PieceType::KNIGHT);
              _('b', Color::BLACK, PieceType::BISHOP);
              _('r', Color::BLACK, PieceType::ROOK);
              _('q', Color::BLACK, PieceType::QUEEN);
              _('k', Color::BLACK, PieceType::KING);
#undef _
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
                file_index += file_contents - '0';
                if (file_index > magic_enum::enum_count<File>()) {
                  return std::unexpected("Too many files.");
                }
                break;
              default:
                return std::unexpected("Invalid piece.");
            }
          }
          if (file_index < magic_enum::enum_count<File>()) {
            return std::unexpected("Too few files.");
          }
        }
        if (rank_index < magic_enum::enum_count<Rank>()) {
          return std::unexpected("Too few ranks.");
        }
        position.board = Board(piece_placement);
        break;
      }
      case 1:
        if (field == "w") {
          position.side_to_move = Color::WHITE;
          break;
        }
        if (field == "b") {
          position.side_to_move = Color::BLACK;
          break;
        }
        return std::unexpected("Invalid active color.");
      case 2:
        if (field.empty()) {
          return std::unexpected("Invalid castling availability.");
        }
        if (field == "-") {
          break;
        }
        for (const auto castling_rights : field) {
          switch (castling_rights) {
#define _(CHAR, CASTLING_RIGHTS)                 \
  case CHAR:                                     \
    position.castling_rights |= CASTLING_RIGHTS; \
    break
            _('K', CastlingRights::WHITE_KINGSIDE);
            _('Q', CastlingRights::WHITE_QUEENSIDE);
            _('k', CastlingRights::BLACK_KINGSIDE);
            _('q', CastlingRights::BLACK_QUEENSIDE);
#undef _
            default:
              return std::unexpected("Invalid castling availability.");
          }
        }
        break;
      case 3:
        if (field == "-") {
          break;
        }
        if (const auto en_passant_target = magic_enum::enum_cast<Square>(field, magic_enum::case_insensitive);
            en_passant_target.transform(rank_of) == [&] {
              switch (position.side_to_move) {
                case Color::WHITE:
                  return Rank::SIX;
                case Color::BLACK:
                  return Rank::THREE;
              }
            }()) {
          position.en_passant_victim_origin = unsafe_shift(to_bitboard(*en_passant_target), [&] {
            switch (position.side_to_move) {
              case Color::WHITE:
                return Direction::SOUTH;
              case Color::BLACK:
                return Direction::NORTH;
            }
          }());
          break;
        }
        return std::unexpected("Invalid en passant target square.");
      case 4:
        if (std::underlying_type_t<Ply> halfmove_clock; from_chars(halfmove_clock)) {
          position.halfmove_clock = Ply{halfmove_clock};
          break;
        }
        return std::unexpected("Invalid halfmove clock.");
      case 5:
        if (from_chars(position.fullmove_number)) {
          break;
        }
        return std::unexpected("Invalid fullmove number.");
      default:
        return std::unexpected("Too many fields.");
    }
  }
  if (field_index < 6) {
    return std::unexpected("Too few fields.");
  }
  return position;
}

inline constexpr auto STARTING_POSITION = parse_fen(STARTING_FEN).value();
}  // namespace prodigy
