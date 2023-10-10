module;

#include <asio/io_context.hpp>

module prodigy.engine;

import prodigy.core;
import prodigy.uci;

namespace prodigy {
Engine::Engine(asio::io_context& io_context) : uci::Engine(io_context) {}

void Engine::set_position(const Position&) {}

void Engine::apply(uci::Move) {}

void Engine::stop() {}
}  // namespace prodigy
