module;

#include <sys/mman.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <memory>
#include <new>
#include <span>
#include <sstream>
#include <system_error>
#include <type_traits>
#include <utility>

export module prodigy.mcts:arena;

export namespace prodigy::mcts {
template <typename T>
concept ArenaAllocatable = alignof(T) == alignof(std::uint64_t) && std::is_trivially_destructible_v<T>;

class Arena {
 public:
  explicit Arena(const std::size_t bytes)
      : arena_([&] {
          static const auto page_size = ::sysconf(_SC_PAGESIZE);
          const auto length = page_size + bytes;
          const auto arena = ::mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
          if (arena == MAP_FAILED) {
            throw std::bad_alloc();
          }
          if (::mprotect(arena, page_size, PROT_NONE)) {
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

  template <ArenaAllocatable T>
  T& new_object(auto&&... args) noexcept
    requires(noexcept(::new (std::declval<void*>()) T(std::forward<decltype(args)>(args)...)))
  {
    assert(ptr_ - arena_.data() >= static_cast<std::ptrdiff_t>(sizeof(T)));
    return *::new (ptr_ -= sizeof(T)) T(std::forward<decltype(args)>(args)...);
  }

  template <ArenaAllocatable T>
  void rollback(T& object) noexcept {
    assert(reinterpret_cast<std::byte*>(std::addressof(object)) >= ptr_);
    assert(reinterpret_cast<std::byte*>(std::addressof(object) + 1) <= arena_.data() + arena_.size());
    ptr_ = reinterpret_cast<std::byte*>(std::addressof(object) + 1);
  }

  std::size_t reset() noexcept { return begin() - std::exchange(ptr_, begin()); }

 private:
  std::byte* begin() const noexcept { return arena_.data() + arena_.size(); }

  const std::span<std::byte> arena_;
  std::byte* ptr_ = begin();
};
}  // namespace prodigy::mcts
