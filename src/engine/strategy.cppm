module;

#include <cstddef>
#include <optional>

export module prodigy.engine:strategy;

import prodigy.core;
import prodigy.uci;

export namespace prodigy {
class Strategy {
 public:
  virtual ~Strategy() = default;

  virtual void start(const Position&, std::optional<std::size_t> nodes) = 0;

  [[nodiscard]] virtual bool poll() = 0;

  virtual void stop() = 0;

  [[nodiscard]] virtual std::optional<uci::Move> join() = 0;
};
}  // namespace prodigy
