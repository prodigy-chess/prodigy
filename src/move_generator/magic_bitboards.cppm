module;

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <magic_enum.hpp>
#include <random>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

module prodigy.move_generator:magic_bitboards;

import prodigy.core;

namespace prodigy::move_generator {
template <Direction... DIRECTIONS>
class MagicBitboards {
 public:
  static void init() noexcept {
    const auto make_attack_set = [](const auto origin, const auto occupancy) {
      Bitboard attack_set{};
      for (const auto direction : {DIRECTIONS...}) {
        for (auto target = shift(to_bitboard(origin), direction); any(target); target = shift(target, direction)) {
          attack_set |= target;
          if (any(occupancy & target)) {
            break;
          }
        }
      }
      return attack_set;
    };
    const auto make_masked_occupancy_to_attack_set = [&](const auto origin, const auto occupancy_mask) {
      std::vector<std::pair<Bitboard, Bitboard>> masked_occupancy_to_attack_set;
      Bitboard subset{};
      do {
        masked_occupancy_to_attack_set.emplace_back(subset, make_attack_set(origin, subset));
        subset = Bitboard{std::to_underlying(subset) - std::to_underlying(occupancy_mask)} & occupancy_mask;
      } while (any(subset));
      return masked_occupancy_to_attack_set;
    };
    auto make_random_sparse_magic = [random_engine = std::default_random_engine(std::random_device()()),
                                     uniform_distribution =
                                         std::uniform_int_distribution<std::underlying_type_t<Magic>>()] mutable {
      return Magic{uniform_distribution(random_engine) & uniform_distribution(random_engine) &
                   uniform_distribution(random_engine)};
    };
    magic_enum::enum_for_each<Square>([&, table = std::span<Bitboard>(table_)](const auto origin) mutable {
      const auto segment = table.first<segment_size(origin)>();
      table = table.subspan(segment.size());
      const auto occupancy_mask = make_occupancy_mask(origin);
      const auto shift = popcount(~occupancy_mask);
      const auto try_fill_segment = [&, masked_occupancy_to_attack_set = make_masked_occupancy_to_attack_set(
                                            origin, occupancy_mask)](const auto magic) {
        std::ranges::fill(segment, Bitboard());
        return std::ranges::all_of(masked_occupancy_to_attack_set, [&](const auto& masked_occupancy_and_attack_set) {
          const auto& [masked_occupancy, attack_set] = masked_occupancy_and_attack_set;
          const auto entry = std::exchange(segment[index(masked_occupancy, occupancy_mask, magic, shift)], attack_set);
          return !any(entry) || entry == attack_set;
        });
      };
      const auto magic = [&] {
        while (true) {
          const auto magic = make_random_sparse_magic();
          if (std::popcount(index(occupancy_mask, occupancy_mask, magic, shift)) < popcount(occupancy_mask) * 2 / 3) {
            continue;
          }
          if (try_fill_segment(magic)) {
            return magic;
          }
        }
      }();
      records_[origin] = {
          .segment = segment.data(),
          .occupancy_mask = occupancy_mask,
          .magic = magic,
          .shift = shift,
      };
    });
  }

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

template class MagicBitboards<Direction::NORTH_EAST, Direction::SOUTH_EAST, Direction::SOUTH_WEST,
                              Direction::NORTH_WEST>;

using RookMagicBitboards = MagicBitboards<Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST>;

template class MagicBitboards<Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST>;
}  // namespace prodigy::move_generator
