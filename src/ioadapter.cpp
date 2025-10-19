#include <stdlib.h>
#include "ioadapter.hpp"

namespace detra {
std::string read(const std::string& filename, std::shared_ptr<IOAdapter> io) {
  int fd = io->open(filename);

  if (fd < 0) return "";

  std::string result;
  ssize_t     size = io->filesize(fd);

  if (size != 0) {
    result.resize(size);
    io->read(fd, &result[0], result.size());
  }
  io->close(fd);

  return result;
}

int write(const std::string& filename, const std::string& content, std::shared_ptr<IOAdapter> io) {
  int fd = io->open(filename);
  if (fd == -1) return 1;

  io->write(fd, content.data(), content.size());
  io->close(fd);

  return 0;
}
} // namespace detra
