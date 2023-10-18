module;

#include <chrono>
#include <cstddef>
#include <optional>
#include <vector>

export module prodigy.uci:go;

import prodigy.core;

import :move;

export namespace prodigy::uci {
struct Go {
  std::vector<Move> search_moves;
  bool ponder = false;
  EnumMap<Color, std::optional<std::chrono::milliseconds>> time_remaining;
  EnumMap<Color, std::chrono::milliseconds> increment{};
  std::optional<std::size_t> moves_to_go;
  std::optional<Ply> depth;
  std::optional<std::size_t> nodes;
  std::optional<std::size_t> mate;
  std::optional<std::chrono::milliseconds> move_time;
  bool infinite = false;

  friend constexpr bool operator==(const Go&, const Go&) = default;
};
}  // namespace prodigy::uci
