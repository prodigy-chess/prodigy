module;

#include <sys/mman.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <new>
#include <span>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <utility>

module prodigy.mcts;

namespace prodigy::mcts {
Arena::Arena(const std::size_t bytes)
    : arena_([&] {
        static const auto page_size = ::sysconf(_SC_PAGE_SIZE);
        const auto length = page_size + bytes;
        const auto arena = ::mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (arena == MAP_FAILED) {
          throw std::bad_alloc();
        }
        if ((reinterpret_cast<std::uintptr_t>(arena) + length) % ALIGNMENT != 0) {
          ::munmap(arena, length);
          throw std::runtime_error((std::ostringstream() << length << " byte arena at " << arena << " doesn't support "
                                                         << ALIGNMENT << " byte alignment")
                                       .str());
        }
        if (::mprotect(arena, page_size, PROT_NONE) == -1) {
          ::munmap(arena, length);
          throw std::system_error(
              std::error_code(errno, std::system_category()),
              (std::ostringstream() << "Failed to mprotect " << page_size << " bytes at " << arena).str());
        }
        return std::span(static_cast<std::byte*>(arena), length);
      }()),
      ptr_(arena_.data() + arena_.size()) {}

Arena::Arena(Arena&& other) noexcept
    : arena_(std::exchange(other.arena_, {})), ptr_(std::exchange(other.ptr_, nullptr)) {}

Arena::~Arena() {
  if (!arena_.empty()) {
    ::munmap(arena_.data(), arena_.size_bytes());
  }
}

std::size_t Arena::size() const noexcept { return arena_.data() + arena_.size() - ptr_; }

void Arena::reset(const std::size_t bytes) noexcept {
  assert(size() >= bytes);
  ptr_ += bytes;
}
}  // namespace prodigy::mcts
