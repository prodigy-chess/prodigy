module;

#include <asio/steady_timer.hpp>
#include <chrono>
#include <optional>

export module prodigy.engine;

import prodigy.core;
import prodigy.uci;

export namespace prodigy {
class Engine : public uci::Engine {
 public:
  explicit Engine(asio::io_context&, std::chrono::steady_clock::duration poll_interval);

 private:
  void set_position(const Position&) override;

  void apply(uci::Move) override;

  void go(const uci::Go&) override;

  void stop() override;

  void async_poll();

  Position position_;
  asio::steady_timer timer_;
  const std::chrono::steady_clock::duration poll_interval_;
  std::optional<const std::chrono::steady_clock::time_point> search_expiry_;
};
}  // namespace prodigy
