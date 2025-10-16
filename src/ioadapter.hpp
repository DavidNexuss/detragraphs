#pragma once
#include <stdlib.h>
#include <memory>

namespace detra {
struct IOAdapter {
  virtual int open()                                           = 0;
  virtual int read(int fd, const char* buffer, size_t length)  = 0;
  virtual int write(int fd, const char* buffer, size_t length) = 0;
  virtual int flush(int fd)                                    = 0;
  virtual int close(int fd)                                    = 0;
  virtual ~IOAdapter() {}
};


struct IOAdapterUnis : public IOAdapter {

  int open() override {}
  int read(int fd, const char* buffer, size_t length) override {}
  int write(int fd, const char* buffer, size_t length) override {}
  int flush(int fd) override {}
  int close(int fd) override {}

  virtual ~IOAdapterUnis() {
  }
};


std::shared_ptr<IOAdapter> unisIO() { return std::make_shared<IOAdapterUnis>(); }
} // namespace detra
