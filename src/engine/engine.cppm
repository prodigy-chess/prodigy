export module prodigy.engine;

import prodigy.core;
import prodigy.uci;

namespace asio {
class io_context;
}

export namespace prodigy {
class Engine : public uci::Engine {
 public:
  explicit Engine(asio::io_context&);

 private:
  void set_position(const Position&) override;

  void apply(uci::Move) override;

  void stop() override;
};
}  // namespace prodigy
