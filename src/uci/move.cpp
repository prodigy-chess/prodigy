module;

#include <cctype>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <utility>

module prodigy.uci;

import prodigy.core;

namespace prodigy::uci {
std::ostream& operator<<(std::ostream& os, const Move move) {
  for (const auto square : {move.origin, move.target}) {
    for (const auto c : magic_enum::enum_name(square)) {
      os << static_cast<char>(std::tolower(c));
    }
  }
  if (move.promotion.has_value()) {
    switch (*move.promotion) {
      case PieceType::KNIGHT:
        os << 'n';
        break;
      case PieceType::BISHOP:
        os << 'b';
        break;
      case PieceType::ROOK:
        os << 'r';
        break;
      case PieceType::QUEEN:
        os << 'q';
        break;
      default:
        std::unreachable();
    }
  }
  return os;
}
}  // namespace prodigy::uci
