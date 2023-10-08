export module prodigy.engine;

import prodigy.uci;

export namespace prodigy {
class Engine : public uci::Engine {
 public:
  void stop() override;
};
}  // namespace prodigy
