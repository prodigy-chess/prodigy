#include <unistd.h>

#include <asio/awaitable.hpp>
#include <asio/buffer.hpp>
#include <asio/co_spawn.hpp>
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>
#include <asio/read_until.hpp>
#include <asio/signal_set.hpp>
#include <asio/use_awaitable.hpp>
#include <chrono>
#include <csignal>
#include <exception>
#include <expected>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

import prodigy.engine;
import prodigy.mcts;
import prodigy.move_generator;

int main() {
  try {
    using namespace prodigy;
    move_generator::init().value();
    asio::io_context io_context(1);
    asio::signal_set signal_set(io_context, SIGINT, SIGTERM);
    signal_set.async_wait([&](const asio::error_code& error, auto&&...) {
      if (!error) {
        io_context.stop();
      }
    });
    asio::co_spawn(
        io_context,
        [&] -> asio::awaitable<void> {
          using namespace std::literals::chrono_literals;
          Engine engine(
              io_context,
              std::make_unique<MCTSStrategy<mcts::EvaluationPolicy, mcts::UCTPolicy>>(
                  std::nullopt, 1ULL << 31, [] { return mcts::EvaluationPolicy(); }, [] { return mcts::UCTPolicy(4); }),
              100ms);
          asio::posix::stream_descriptor input(io_context, ::dup(STDIN_FILENO));
          std::string buffer;
          while (true) {
            const auto bytes_transferred =
                co_await asio::async_read_until(input, asio::dynamic_buffer(buffer), '\n', asio::use_awaitable);
            const auto line = std::string_view(buffer).substr(0, bytes_transferred - 1);
            try {
              engine.handle(line);
            } catch (const std::bad_expected_access<std::string_view>& exception) {
              std::clog << "Error handling " << std::quoted(line) << ": " << exception.error() << '\n';
            } catch (const std::exception& exception) {
              std::clog << "Error handling " << std::quoted(line) << ": " << exception.what() << '\n';
            } catch (...) {
              std::clog << "Error handling " << std::quoted(line) << '\n';
            }
            buffer.erase(0, bytes_transferred);
          }
        },
        [](const std::exception_ptr exception_ptr) {
          if (exception_ptr) {
            std::rethrow_exception(exception_ptr);
          }
        });
    io_context.run();
  } catch (const std::exception& exception) {
    std::clog << exception.what() << '\n';
    return 1;
  }
}
