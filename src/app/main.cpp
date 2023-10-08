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
#include <csignal>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

import prodigy.engine;
import prodigy.uci;

int main() {
  try {
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
          asio::posix::stream_descriptor input(io_context, ::dup(STDIN_FILENO));
          std::string buffer;
          const prodigy::uci::Parser parser(std::make_unique<prodigy::Engine>());
          while (true) {
            const auto bytes_transferred =
                co_await asio::async_read_until(input, asio::dynamic_buffer(buffer), '\n', asio::use_awaitable);
            const auto end = buffer.begin() + bytes_transferred;
            if (!parser.handle({buffer.begin(), end})) {
              io_context.stop();
            }
            buffer.erase(buffer.begin(), end);
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
