#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_set>
#include "../ioadapter.hpp"
#include <iostream>

namespace graphs {

namespace backends {

template <typename edgeType = bool>
struct AdjacencyMatrix {
  std::vector<std::vector<edgeType>> mat;

  uint64_t getVertexCount() const { return mat.size(); }

  uint64_t getEdgeCount() const {
    uint64_t c = 0;
    for (auto& row : mat)
      for (auto e : row)
        if (e) ++c;
    return c;
  }

  uint64_t getEdgeCount(uint64_t vertex) const {
    uint64_t c = 0;
    for (auto& e : mat[vertex])
      if (e) ++c;
    return c;
  }

  void addEdge(uint64_t from, uint64_t to) {
    if (from == to) return;
    mat[from][to] = true;
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    return mat[from][to];
  }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void addVertices(uint64_t vertices) {
    size_t old_size = mat.size();
    for (auto& row : mat) row.resize(old_size + vertices);
    mat.resize(old_size + vertices, std::vector<edgeType>(old_size + vertices));
  }

  void print() {
    std::cout << "---AdjacencyMatrix---" << std::endl;
    for (auto& row : mat) {
      for (const auto& val : row) {
        std::cout << val << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
};

template <typename edgeType = bool>
struct AdjacencyMatrixFlat {
  std::vector<edgeType> mat;

  size_t N = 0;

  uint64_t getVertexCount() const { return N; }

  uint64_t getEdgeCount() const {
    uint64_t c = 0;
    for (auto e : mat)
      if (e) ++c;
    return c;
  }

  uint64_t getEdgeCount(uint64_t vertex) const {
    uint64_t c     = 0;
    size_t   start = vertex * N;
    for (size_t i = 0; i < N; ++i)
      if (mat[start + i]) ++c;
    return c;
  }

  void addEdge(uint64_t from, uint64_t to) {
    if (from == to) return;
    mat[from * N + to] = true;
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    return mat[from * N + to];
  }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}
  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void addVertices(uint64_t vertices) {
    size_t oldN = N;
    N += vertices;
    std::vector<edgeType> newMat(N * N, 0);
    for (size_t i = 0; i < oldN; ++i)
      for (size_t j = 0; j < oldN; ++j)
        newMat[i * N + j] = mat[i * oldN + j];
    mat = std::move(newMat);
  }

  void print() {
    std::cout << "---AdjacencyMatrixFlat " << N << "---" << std::endl;
    if (N == 0) return;

    for (int i = 0; i < mat.size(); i++) {
      if ((i % N) == 0) std::cout << std::endl;
      std::cout << int(mat[i]) << " ";
    }
    std::cout << std::endl;
  }
};

struct AdjacencyMatrixRange {
  std::vector<std::vector<std::pair<uint64_t, uint64_t>>> ranges;

  uint64_t getVertexCount() const { return ranges.size(); }

  void addVertices(uint64_t vertices) {
    ranges.resize(ranges.size() + vertices, {});
  }

  void addEdge(uint64_t from, uint64_t to) {
    if (from == to) return;
    auto& vec = ranges[from];
    if (!vec.empty() && vec.back().second + 1 == to)
      vec.back().second = to; // extend last range
    else
      vec.emplace_back(to, to);
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    const auto& vec = ranges[from];
    for (auto& r : vec)
      if (to >= r.first && to <= r.second) return true;
    return false;
  }

  uint64_t getEdgeCount() const {
    uint64_t c = 0;
    for (auto& row : ranges)
      for (auto& r : row)
        c += r.second - r.first + 1;
    return c;
  }

  uint64_t getEdgeCount(uint64_t vertex) const {
    uint64_t c = 0;
    for (auto& r : ranges[vertex])
      c += r.second - r.first + 1;
    return c;
  }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}
  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void print() {
    std::cout << "---AdjacencyMatrixRange---" << std::endl;
    for (int i = 0; i < ranges.size(); i++) {
      std::cout << i << ": ";
      for (auto& range : ranges[i]) {
        std::cout << "(" << range.first << "," << range.second << ")" << " ";
      }

      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
};

struct AdjacencyMatrixHash {

  struct pair_hash {
    std::size_t operator()(const std::pair<uint64_t, uint64_t>& p) const noexcept {
      std::size_t h1 = std::hash<uint64_t>{}(p.first);
      std::size_t h2 = std::hash<uint64_t>{}(p.second);
      return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
  };

  std::unordered_set<std::pair<uint64_t, uint64_t>, pair_hash, std::equal_to<>> edges;

  size_t N = 0;

  uint64_t getVertexCount() const { return N; }
  uint64_t getEdgeCount() const { return edges.size(); }

  uint64_t getEdgeCount(uint64_t vertex) const {
    uint64_t c = 0;
    for (auto& e : edges)
      if (e.first == vertex) ++c;
    return c;
  }

  void addEdge(uint64_t from, uint64_t to) {
    if (from == to) return;
    edges.emplace(from, to);
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    return edges.find({from, to}) != edges.end();
  }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}
  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void addVertices(uint64_t vertices) { N += vertices; }

  void print() {
    std::cout << "---AdjacencyMatrixHash---" << std::endl;
    for (auto& edge : edges) {
      std::cout << "{" << edge.first << "," << edge.second << "}" << " ";
    }
    std::cout << std::endl;
  }
};

} // namespace backends

} // namespace graphs
