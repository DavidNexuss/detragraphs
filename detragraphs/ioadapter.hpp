#pragma once
#include <stdlib.h>
#include <memory>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

namespace detra {
struct IOAdapter {
  virtual int      open(const std::string& filename)                = 0;
  virtual uint64_t filesize(int fd)                                 = 0;
  virtual ssize_t  read(int fd, void* buffer, size_t length)        = 0;
  virtual ssize_t  write(int fd, const void* buffer, size_t length) = 0;
  virtual int      flush(int fd)                                    = 0;
  virtual int      close(int fd)                                    = 0;
  virtual ~IOAdapter() {}
};

struct IOAdapterUnis : public IOAdapter {
  static constexpr size_t kMaxBuffer = 256 * 1024 * 1024; // 256 MiB

  inline int open(const std::string& filename) override {
    return ::open(filename.c_str(), O_RDWR | O_CREAT | O_SYNC, 0644);
  }

  inline uint64_t filesize(int fd) override {
    struct stat st{};
    if (::fstat(fd, &st) != 0) return 0;
    return static_cast<uint64_t>(st.st_size);
  }

  inline ssize_t read(int fd, void* buffer, size_t length) override {
    size_t total = 0;
    auto*  ptr   = static_cast<char*>(buffer);

    while (total < length) {
      size_t  chunk = (length - total) > kMaxBuffer ? kMaxBuffer : (length - total);
      ssize_t r     = ::read(fd, ptr + total, chunk);
      if (r < 0) {
        if (errno == EINTR) continue;
        return -1;
      }
      if (r == 0) break; // EOF
      total += r;
    }
    return total;
  }

  inline ssize_t write(int fd, const void* buffer, size_t length) override {
    size_t total = 0;
    auto*  ptr   = static_cast<const char*>(buffer);

    while (total < length) {
      size_t  chunk = (length - total) > kMaxBuffer ? kMaxBuffer : (length - total);
      ssize_t w     = ::write(fd, ptr + total, chunk);
      if (w < 0) {
        if (errno == EINTR) continue;
        return -1;
      }
      total += w;
    }
    return total;
  }

  inline int flush(int fd) override { return ::fsync(fd); }
  inline int close(int fd) override { return ::close(fd); }

  ~IOAdapterUnis() override = default;
};


std::shared_ptr<IOAdapter> unisIO() { return std::make_shared<IOAdapterUnis>(); }
} // namespace detra
