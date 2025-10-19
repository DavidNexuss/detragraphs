#pragma once
#include <cstdint>
#include <vector>

namespace graphs {

enum class GraphType {
  DIRECTED,
  UNDIRECTED
};

template <typename Backend>
struct Graph {
  Backend   data;
  GraphType type = GraphType::DIRECTED;

  inline uint64_t getVertexCount() const { return data.getVertexCount(); }
  inline uint64_t getEdgeCount() const { return data.getEdgeCount(); }
  inline uint64_t getEdgeCount(uint64_t vertex) const { return data.getEdgeCount(vertex); }

  inline void setType(GraphType type) {
    this->type = type;
  }

  inline void addEdge(uint64_t from, uint64_t to) {
    data.addEdge(from, to);
    if (type == GraphType::UNDIRECTED) {
      data.addEdge(to, from);
    }
  }

  inline bool isConnected(uint64_t from, uint64_t to) const { return data.isConnected(from, to); }

  inline bool isConnectedUndirected(uint64_t from, uint64_t to) const {
    if (to > from) std::swap(to, from);
    return data.isConnected(from, to);
  }
  inline void addVertices(uint64_t vertices) { data.addVertices(vertices); }

  inline std::vector<uint64_t> getEdges(uint64_t vertex) const { return data.getEdges(vertex); }

  inline void print() {
    data.print();
  }

  inline float getEdgeWeight(uint64_t u, uint64_t v) const {
    return 1.0f;
  }
};
} // namespace graphs
