#include <stdlib.h>
#include "ioadapter.hpp"
#include <vector>

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

std::map<std::string, std::string> readdir(const std::string& directory, std::shared_ptr<IOAdapter> io = unisIO()) {
  std::map<std::string, std::string> result;

  std::vector<std::string> files = io->listdir(directory);

  for (const auto& file : files) {
    std::string path = directory;
    if (!path.empty() && path.back() != '/') path += '/';
    path += file;

    std::string content = io::read(path, io);
    result[file]        = content;
  }

  return result;
}
} // namespace detra
