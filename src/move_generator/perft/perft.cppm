module;

#include <cstdint>
#include <expected>
#include <map>
#include <string_view>

export module prodigy.move_generator.perft;

import prodigy.core;
import prodigy.uci;

export namespace prodigy::move_generator::perft {
std::expected<std::uint64_t, std::string_view> perft(const Position&, Ply) noexcept;

std::expected<std::map<uci::Move, std::uint64_t>, std::string_view> divide(const Position&, Ply) noexcept;
}  // namespace prodigy::move_generator::perft
