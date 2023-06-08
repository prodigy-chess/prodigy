module;

#include <expected>
#include <string_view>

export module prodigy.move_generator;

export namespace prodigy::move_generator {
[[nodiscard]] std::expected<void, std::string_view> init() noexcept;
}
