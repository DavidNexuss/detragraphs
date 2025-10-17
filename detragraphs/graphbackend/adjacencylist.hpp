#pragma once
#include <vector>
#include <unordered_set>
#include <cstdint>
#include <string>
#include <algorithm>
#include "../ioadapter.hpp"
#include <iostream>

namespace graphs {

namespace backends {
struct AdjacencyListVector {
  std::vector<std::vector<uint64_t>> adj;

  uint64_t getVertexCount() const { return adj.size(); }
  uint64_t getEdgeCount() const {
    uint64_t c = 0;
    for (auto& v : adj) c += v.size();
    return c;
  }
  uint64_t getEdgeCount(uint64_t v) const { return adj[v].size(); }

  void addEdge(uint64_t from, uint64_t to) {
    if (from == to) return;
    if (std::find(adj[from].begin(), adj[from].end(), to) == adj[from].end())
      adj[from].push_back(to);
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    return std::find(adj[from].begin(), adj[from].end(), to) != adj[from].end();
  }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}
  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void addVertices(uint64_t vertices) {
    adj.resize(adj.size() + vertices, {});
  }

  void print() {
    std::cout << "---AdjacencyList---" << std::endl;
    for (size_t i = 0; i < adj.size(); i++) {
      std::cout << i << ": ";
      for (size_t j = 0; j < adj[i].size(); j++) {
        std::cout << adj[i][j] << " ";
      }
      std::cout << std::endl;
    }
  }
};

struct AdjacencyListHash {
  std::vector<std::unordered_set<uint64_t>> adj;

  uint64_t getVertexCount() const { return adj.size(); }
  uint64_t getEdgeCount() const {
    uint64_t c = 0;
    for (auto& v : adj) c += v.size();
    return c;
  }
  uint64_t getEdgeCount(uint64_t v) { return adj[v].size(); }

  void addEdge(uint64_t from, uint64_t to) {
    if (from == to) return;
    adj[from].insert(to);
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    return adj[from].count(to) > 0;
  }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}
  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void addVertices(uint64_t vertices) {
    adj.resize(adj.size() + vertices);
  }

  void print() {}
};

struct AdjacencyListSorted {
  std::vector<std::vector<uint64_t>> adj;

  uint64_t getVertexCount() const { return adj.size(); }
  uint64_t getEdgeCount() const {
    uint64_t c = 0;
    for (auto& v : adj) c += v.size();
    return c;
  }
  uint64_t getEdgeCount(uint64_t v) { return adj[v].size(); }

  void addEdge(uint64_t from, uint64_t to) {
    if (from == to) return;
    auto& vec = adj[from];
    auto  it  = std::lower_bound(vec.begin(), vec.end(), to);
    if (it == vec.end() || *it != to) vec.insert(it, to);
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    const auto& vec = adj[from];
    return std::binary_search(vec.begin(), vec.end(), to);
  }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}
  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void addVertices(uint64_t vertices) {
    adj.resize(adj.size() + vertices, {});
  }

  void print() {}
};

struct AdjacencyListFlat {
  std::vector<uint64_t> edges;
  std::vector<size_t>   offsets; // offsets[i] = start of vertex i's edges

  uint64_t getVertexCount() const { return offsets.size(); }

  uint64_t getEdgeCount() const {
    return edges.size();
  }

  uint64_t getEdgeCount(uint64_t v) const {
    if (v + 1 < offsets.size()) return offsets[v + 1] - offsets[v];
    if (v < offsets.size()) return edges.size() - offsets[v];
    return 0;
  }

  void addEdge(uint64_t from, uint64_t to) {
    if (from >= offsets.size() || from == to) return;
    edges.insert(edges.begin() + offsets[from + 1], to); // simplistic, real impl may require shift
    for (size_t i = from + 1; i < offsets.size(); ++i) offsets[i]++;
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    if (from >= offsets.size()) return false;
    size_t start = offsets[from];
    size_t end   = (from + 1 < offsets.size()) ? offsets[from + 1] : edges.size();
    return std::find(edges.begin() + start, edges.begin() + end, to) != edges.begin() + end;
  }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}
  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void addVertices(uint64_t vertices) {
    size_t old = offsets.size();
    offsets.resize(old + vertices, edges.size());
  }
  void print() {
  }
};

} // namespace backends

} // namespace graphs
