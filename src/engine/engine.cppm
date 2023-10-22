module;

#include <asio/steady_timer.hpp>
#include <chrono>
#include <memory>
#include <optional>

export module prodigy.engine;

export import :mcts_strategy;
export import :strategy;

import prodigy.core;
import prodigy.uci;

export namespace prodigy {
class Engine final : public uci::Engine {
 public:
  explicit Engine(asio::io_context&, std::unique_ptr<Strategy>, std::chrono::steady_clock::duration poll_interval);

 private:
  void set_position(const Position&) override;

  void apply(uci::Move) override;

  void go(const uci::Go&) override;

  void stop() override;

  void async_poll();

  Position position_;
  asio::steady_timer timer_;
  const std::unique_ptr<Strategy> strategy_;
  const std::chrono::steady_clock::duration poll_interval_;
  std::optional<const std::chrono::steady_clock::time_point> search_expiry_;
};
}  // namespace prodigy
