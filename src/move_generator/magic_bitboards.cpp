module;

#include <algorithm>
#include <bit>
#include <magic_enum/magic_enum_utility.hpp>
#include <random>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

module prodigy.move_generator;

import prodigy.core;

namespace prodigy::move_generator {
template <Direction... DIRECTIONS>
void MagicBitboards<DIRECTIONS...>::init() noexcept {
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
        return empty(entry) || entry == attack_set;
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

template class MagicBitboards<Direction::NORTH_EAST, Direction::SOUTH_EAST, Direction::SOUTH_WEST,
                              Direction::NORTH_WEST>;

template class MagicBitboards<Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST>;
}  // namespace prodigy::move_generator
