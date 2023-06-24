#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstdint>
#include <string_view>

import prodigy.core;
import prodigy.move_generator;

namespace prodigy::move_generator {
namespace {
struct MoveCounts {
  std::uint64_t knight_quiet_moves = 0;
  std::uint64_t knight_captures = 0;
  std::uint64_t bishop_quiet_moves = 0;
  std::uint64_t bishop_captures = 0;
  std::uint64_t rook_quiet_moves = 0;
  std::uint64_t rook_captures = 0;
  std::uint64_t queen_quiet_moves = 0;
  std::uint64_t queen_captures = 0;
  std::uint64_t king_quiet_moves = 0;
  std::uint64_t king_captures = 0;
  std::uint64_t kingside_castles = 0;
  std::uint64_t queenside_castles = 0;

  friend constexpr bool operator==(const MoveCounts&, const MoveCounts&) = default;
};

class Visitor : public move_generator::Visitor<Visitor> {
 public:
  constexpr explicit Visitor(MoveCounts& move_counts) noexcept : move_counts_(move_counts) {}

  template <Node::Context>
  constexpr void visit_knight_move(const QuietMove&) const noexcept {
    ++move_counts_.knight_quiet_moves;
  }

  template <Node::Context>
  constexpr void visit_knight_move(const Capture&) const noexcept {
    ++move_counts_.knight_captures;
  }

  template <Node::Context>
  constexpr void visit_bishop_move(const QuietMove&) const noexcept {
    ++move_counts_.bishop_quiet_moves;
  }

  template <Node::Context>
  constexpr void visit_bishop_move(const Capture&) const noexcept {
    ++move_counts_.bishop_captures;
  }

  template <Node::Context>
  constexpr void visit_rook_move(const QuietMove&) const noexcept {
    ++move_counts_.rook_quiet_moves;
  }

  template <Node::Context>
  constexpr void visit_rook_move(const Capture&) const noexcept {
    ++move_counts_.rook_captures;
  }

  template <Node::Context>
  constexpr void visit_queen_move(const QuietMove&) const noexcept {
    ++move_counts_.queen_quiet_moves;
  }

  template <Node::Context>
  constexpr void visit_queen_move(const Capture&) const noexcept {
    ++move_counts_.queen_captures;
  }

  template <Node::Context>
  constexpr void visit_king_move(const QuietMove&) const noexcept {
    ++move_counts_.king_quiet_moves;
  }

  template <Node::Context>
  constexpr void visit_king_move(const Capture&) const noexcept {
    ++move_counts_.king_captures;
  }

  template <Node::Context context>
  constexpr void visit_king_move(const Castle& move) const noexcept {
    move.rook_origin == ColorTraits<!context.side_to_move>::template CastlingTraits<PieceType::KING>::CASTLE.rook_origin
        ? ++move_counts_.kingside_castles
        : ++move_counts_.queenside_castles;
  }

 private:
  MoveCounts& move_counts_;
};

TEST_CASE("walk") {
  const auto [fen, context, expected_move_counts] = GENERATE(table<std::string_view, std::string_view, MoveCounts>({
      {
          "7k/8/8/8/8/7r/K7/8 w - - 0 1",
          "king quiet moves",
          {
              .king_quiet_moves = 3,
          },
      },
      {
          "7k/8/8/8/8/2r5/1K6/nn6 w - - 0 1",
          "king captures",
          {
              .king_quiet_moves = 1,
              .king_captures = 2,
          },
      },
      {
          "6k1/8/8/8/4r3/8/8/4K2R w K - 0 1",
          "can't kingside castle out of check",
          {
              .king_quiet_moves = 4,
          },
      },
      {
          "6k1/8/8/8/5r2/8/8/4K2R w K - 0 1",
          "can't kingside castle through check",
          {
              .rook_quiet_moves = 9,
              .king_quiet_moves = 3,
          },
      },
      {
          "6k1/8/8/8/6r1/8/8/4K2R w K - 0 1",
          "can't kingside castle into check",
          {
              .rook_quiet_moves = 9,
              .king_quiet_moves = 5,
          },
      },
      {
          "6k1/8/8/8/8/8/8/4K2R w K - 0 1",
          "kingside castle",
          {
              .rook_quiet_moves = 9,
              .king_quiet_moves = 5,
              .kingside_castles = 1,
          },
      },
      {
          "6k1/8/8/8/8/8/8/4K2R w - - 0 1",
          "kingside castle requires castling rights",
          {
              .rook_quiet_moves = 9,
              .king_quiet_moves = 5,
          },
      },
      {
          "7k/8/8/8/4r3/8/8/R3K3 w Q - 0 1",
          "can't queenside castle out of check",
          {
              .king_quiet_moves = 4,
          },
      },
      {
          "7k/8/8/8/3r4/8/8/R3K3 w Q - 0 1",
          "can't queenside castle through check",
          {
              .rook_quiet_moves = 10,
              .king_quiet_moves = 3,
          },
      },
      {
          "7k/8/8/8/2r5/8/8/R3K3 w Q - 0 1",
          "can't queenside castle into check",
          {
              .rook_quiet_moves = 10,
              .king_quiet_moves = 5,
          },
      },
      {
          "7k/8/8/8/1r6/8/8/R3K3 w Q - 0 1",
          "queenside castle",
          {
              .rook_quiet_moves = 10,
              .king_quiet_moves = 5,
              .queenside_castles = 1,
          },
      },
      {
          "7k/8/8/8/8/8/8/R3K3 w - - 0 1",
          "queenside castle requires castling rights",
          {
              .rook_quiet_moves = 10,
              .king_quiet_moves = 5,
          },
      },
      {
          "7k/8/8/8/r7/8/1N6/3K4 w - - 0 1",
          "unpinned knight moves",
          {
              .knight_quiet_moves = 2,
              .knight_captures = 1,
              .king_quiet_moves = 5,
          },
      },
      {
          "3r3k/8/8/3N3b/8/5N2/8/3K4 w - - 0 1",
          "pinned knight can't move",
          {
              .king_quiet_moves = 5,
          },
      },
      {
          "7k/8/8/4N3/6b1/8/8/3K4 w - - 0 1",
          "knight check evasion moves",
          {
              .knight_quiet_moves = 1,
              .knight_captures = 1,
              .king_quiet_moves = 4,
          },
      },
      {
          "7k/8/8/8/4p3/5B2/8/3K4 w - - 0 1",
          "unpinned bishop moves",
          {
              .bishop_quiet_moves = 5,
              .bishop_captures = 1,
              .king_quiet_moves = 5,
          },
      },
      {
          "3r3k/8/8/3B4/6b1/8/4B3/3K4 w - - 0 1",
          "pinned bishop moves",
          {
              .bishop_quiet_moves = 1,
              .bishop_captures = 1,
              .king_quiet_moves = 4,
          },
      },
      {
          "7k/8/8/6q1/5B2/5B2/8/2K4r w - - 0 1",
          "bishop check evasion moves",
          {
              .bishop_quiet_moves = 1,
              .bishop_captures = 1,
              .king_quiet_moves = 3,
          },
      },
      {
          "7k/8/8/6p1/8/8/8/4K1R1 w - - 0 1",
          "unpinned rook moves",
          {
              .rook_quiet_moves = 5,
              .rook_captures = 1,
              .king_quiet_moves = 5,
          },
      },
      {
          "7k/8/4q3/4R3/7b/6R1/8/4K3 w - - 0 1",
          "pinned rook moves",
          {
              .rook_quiet_moves = 3,
              .rook_captures = 1,
              .king_quiet_moves = 5,
          },
      },
      {
          "7k/8/8/8/1q2R2b/2R5/R7/4K3 w - - 0 1",
          "rook check evasion moves",
          {
              .rook_quiet_moves = 1,
              .rook_captures = 1,
              .king_quiet_moves = 4,
          },
      },
      {
          "7k/8/8/2p5/8/4Q3/8/4K3 w - - 0 1",
          "unpinned queen moves",
          {
              .queen_quiet_moves = 21,
              .queen_captures = 1,
              .king_quiet_moves = 5,
          },
      },
      {
          "2r3bk/2Q5/4Q3/8/2K1Q2r/8/8/8 w - - 0 1",
          "pinned queen moves",
          {
              .queen_quiet_moves = 7,
              .queen_captures = 3,
              .king_quiet_moves = 8,
          },
      },
      {
          "2r3bk/2Q1QQ2/8/8/2K4r/8/8/8 w - - 0 1",
          "queen check evasion moves",
          {
              .queen_quiet_moves = 1,
              .queen_captures = 1,
              .king_quiet_moves = 6,
          },
      },
  }));
  static_cast<void>(init());
  INFO(fen);
  INFO(context);
  dispatch(parse_fen(fen).value(), [&]<auto context>(const auto& node) {
    MoveCounts move_counts;
    walk<context>(node, Visitor(move_counts));
    REQUIRE(move_counts == expected_move_counts);
  });
}
}  // namespace
}  // namespace prodigy::move_generator
