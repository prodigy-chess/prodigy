module;

#include <string_view>

export module prodigy.uci:engine;

import prodigy.core;

import :move;

export namespace prodigy::uci {
class Engine {
 public:
  virtual ~Engine() = default;

  [[nodiscard]] bool handle(std::string_view);

 private:
  virtual void set_position(const Position&) = 0;

  virtual void apply(Move) = 0;

  virtual void stop() = 0;
};
}  // namespace prodigy::uci
