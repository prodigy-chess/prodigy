export module prodigy.engine;

import prodigy.core;
import prodigy.uci;

export namespace prodigy {
class Engine : public uci::Engine {
 private:
  void set_position(const Position&) override;

  void apply(uci::Move) override;

  void stop() override;
};
}  // namespace prodigy
