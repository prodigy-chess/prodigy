module;

#include <cassert>
#include <cstddef>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>

export module prodigy.mcts:arena;

export namespace prodigy::mcts {
class Arena {
 public:
  static constexpr auto ALIGNMENT = 8UZ;

  explicit Arena(std::size_t bytes);

  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  Arena(Arena&&) = delete;
  Arena& operator=(Arena&&) = delete;

  ~Arena();

  std::size_t size() const noexcept;

  template <typename T>
  T& new_object(auto&&... args) noexcept
    requires(alignof(T) == ALIGNMENT && std::is_trivially_destructible_v<T> &&
             noexcept(::new (std::declval<void*>()) T(std::forward<decltype(args)>(args)...)))
  {
    assert(ptr_ - arena_.data() >= static_cast<std::ptrdiff_t>(sizeof(T)));
    return *::new (std::assume_aligned<alignof(T)>(ptr_ -= sizeof(T))) T(std::forward<decltype(args)>(args)...);
  }

  void reset(std::size_t bytes) noexcept;

 private:
  const std::span<std::byte> arena_;
  std::byte* ptr_ = arena_.data() + arena_.size();
};
}  // namespace prodigy::mcts
