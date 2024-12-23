module;

#include <array>
#include <cstddef>
#include <cstdint>
#include <magic_enum/magic_enum_utility.hpp>
#include <utility>

module prodigy.move_generator:magic_bitboards;

import prodigy.core;

namespace prodigy::move_generator {
template <Direction... DIRECTIONS>
class MagicBitboards {
 public:
  static void init() noexcept;

  static Bitboard attack_set(const Square origin, const Bitboard occupancy) noexcept {
    const auto& [segment, occupancy_mask, magic, shift] = records_[origin];
    return segment[index(occupancy, occupancy_mask, magic, shift)];
  }

 private:
  enum class Magic : std::uint64_t {};

  struct Record {
    const Bitboard* segment;
    Bitboard occupancy_mask;
    Magic magic;
    std::uint8_t shift;
  };

  static constexpr std::size_t index(const Bitboard occupancy, const Bitboard occupancy_mask, const Magic magic,
                                     const std::uint8_t shift) noexcept {
    return std::to_underlying(occupancy & occupancy_mask) * std::to_underlying(magic) >> shift;
  }

  static constexpr Bitboard make_occupancy_mask(const Square origin) noexcept {
    Bitboard occupancy_mask{};
    for (const auto direction : {DIRECTIONS...}) {
      for (auto target = shift(to_bitboard(origin), direction); any(shift(target, direction));
           target = shift(target, direction)) {
        occupancy_mask |= target;
      }
    }
    return occupancy_mask;
  }

  static constexpr std::size_t segment_size(const Square origin) noexcept {
    return 1UZ << popcount(make_occupancy_mask(origin));
  }

  inline static std::array<Bitboard,
                           [] {
                             auto size = 0UZ;
                             magic_enum::enum_for_each<Square>(
                                 [&](const auto origin) { size += segment_size(origin); });
                             return size;
                           }()>
      table_;
  inline static EnumMap<Square, Record> records_;
};

using BishopMagicBitboards =
    MagicBitboards<Direction::NORTH_EAST, Direction::SOUTH_EAST, Direction::SOUTH_WEST, Direction::NORTH_WEST>;

extern template class MagicBitboards<Direction::NORTH_EAST, Direction::SOUTH_EAST, Direction::SOUTH_WEST,
                                     Direction::NORTH_WEST>;

using RookMagicBitboards = MagicBitboards<Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST>;

extern template class MagicBitboards<Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST>;
}  // namespace prodigy::move_generator
