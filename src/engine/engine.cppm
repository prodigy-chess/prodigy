module;

#include <asio/steady_timer.hpp>

export module prodigy.engine;

import prodigy.core;
import prodigy.uci;

export namespace prodigy {
class Engine : public uci::Engine {
 public:
  explicit Engine(asio::io_context&);

 private:
  void set_position(const Position&) override;

  void apply(uci::Move) override;

  void go(const uci::Go&) override;

  void stop() override;

  asio::steady_timer timer_;
};
}  // namespace prodigy
