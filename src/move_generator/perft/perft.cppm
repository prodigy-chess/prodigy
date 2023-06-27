module;

#include <cstdint>
#include <expected>
#include <string_view>

export module prodigy.move_generator.perft;

import prodigy.core;

export namespace prodigy::move_generator::perft {
std::expected<std::uint64_t, std::string_view> perft(const Position&, Ply);
}
