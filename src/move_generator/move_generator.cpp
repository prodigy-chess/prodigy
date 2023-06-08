module;

#include <expected>
#include <string_view>
#include <utility>

module prodigy.move_generator;

import :magic_bitboards;

namespace prodigy::move_generator {
std::expected<void, std::string_view> init() noexcept {
  if (static bool initialized = false; std::exchange(initialized, true)) {
    return std::unexpected("Already initialized.");
  }
  BishopMagicBitboards::init();
  RookMagicBitboards::init();
  return {};
}
}  // namespace prodigy::move_generator
