module;

#include <string_view>

export module prodigy.uci:engine;

import prodigy.core;

import :go;
import :move;

namespace asio {
class io_context;
}

export namespace prodigy::uci {
class Engine {
 public:
  explicit Engine(asio::io_context&) noexcept;

  virtual ~Engine() = default;

  void handle(std::string_view);

 private:
  virtual void set_position(const Position&) = 0;

  virtual void apply(Move) = 0;

  virtual void go(const Go&) = 0;

  virtual void stop() = 0;

  asio::io_context& io_context_;
};
}  // namespace prodigy::uci
