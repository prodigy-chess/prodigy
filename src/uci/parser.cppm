module;

#include <memory>
#include <string_view>

export module prodigy.uci:parser;

import :engine;

export namespace prodigy::uci {
class Parser {
 public:
  explicit Parser(std::unique_ptr<Engine>) noexcept;

  [[nodiscard]] bool handle(std::string_view) const noexcept;

 private:
  const std::unique_ptr<Engine> engine_;
};
}  // namespace prodigy::uci
