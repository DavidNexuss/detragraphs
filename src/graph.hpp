#pragma once
#include "ioadapter.hpp"
#include <cstdint>
#include <memory>
#include "graphbackend.hpp"

namespace graphs {

template <typename Backend>
struct Graph {
  Backend data;

  inline uint64_t getVertexCount() const { return data.getVertexCount(); }
  inline uint64_t getEdgeCount() const { return data.getEdgeCount(); }

  inline void addEdge(uint64_t from, uint64_t to) { data.addEdge(from, to); }
  inline bool isConnected(uint64_t from, uint64_t to) const { return data.isConnected(from, to); }
  inline void addVertices(uint64_t vertices) { data.addVertices(vertices); }

  void writedisk(const std::string& path, std::shared_ptr<detra::IOAdapter> io = detra::unisIO()) {
    data.writedisk(path, io);
  }

  void readdisk(const std::string& path, std::shared_ptr<detra::IOAdapter> io = detra::unisIO()) {
    data.readdisk(path, io);
  }
};
} // namespace graphs
