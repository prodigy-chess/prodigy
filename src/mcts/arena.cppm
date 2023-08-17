module;

#include <sys/mman.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <span>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <utility>

export module prodigy.mcts:arena;

export namespace prodigy::mcts {
class Arena {
 public:
  static constexpr auto ALIGNMENT = 8UZ;

  explicit Arena(const std::size_t bytes)
      : arena_([&] {
          static const auto page_size = ::sysconf(_SC_PAGE_SIZE);
          const auto length = page_size + bytes;
          const auto arena = ::mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
          if (arena == MAP_FAILED) {
            throw std::bad_alloc();
          }
          if ((reinterpret_cast<std::uintptr_t>(arena) + length) % ALIGNMENT != 0) {
            ::munmap(arena, length);
            throw std::runtime_error((std::ostringstream() << length << " byte arena at " << arena
                                                           << " doesn't support " << ALIGNMENT << " byte alignment")
                                         .str());
          }
          if (::mprotect(arena, page_size, PROT_NONE) == -1) {
            ::munmap(arena, length);
            throw std::system_error(
                std::error_code(errno, std::system_category()),
                (std::ostringstream() << "Failed to mprotect " << page_size << " bytes at " << arena).str());
          }
          return std::span(static_cast<std::byte*>(arena), length);
        }()) {}

  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  Arena(Arena&&) = delete;
  Arena& operator=(Arena&&) = delete;

  ~Arena() { ::munmap(arena_.data(), arena_.size_bytes()); }

  std::size_t size() const noexcept { return arena_.data() + arena_.size() - ptr_; }

  template <typename T>
  T& new_object(auto&&... args) noexcept
    requires(alignof(T) == ALIGNMENT && std::is_trivially_destructible_v<T> &&
             noexcept(::new (std::declval<void*>()) T(std::forward<decltype(args)>(args)...)))
  {
    assert(ptr_ - arena_.data() >= static_cast<std::ptrdiff_t>(sizeof(T)));
    return *::new (std::assume_aligned<alignof(T)>(ptr_ -= sizeof(T))) T(std::forward<decltype(args)>(args)...);
  }

  void reset(const std::size_t bytes) noexcept {
    assert(size() >= bytes);
    ptr_ += bytes;
  }

 private:
  const std::span<std::byte> arena_;
  std::byte* ptr_ = arena_.data() + arena_.size();
};
}  // namespace prodigy::mcts
