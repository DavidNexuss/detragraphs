#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>
#include "ioadapter.hpp"

namespace graphs {

namespace backends {

struct AdjacencyList {
  std::vector<std::vector<uint64_t>> adj;

  uint64_t getVertexCount() const { return adj.size(); }
  uint64_t getEdgeCount() const {
    uint64_t c = 0;
    for (auto& v : adj) c += v.size();
    return c;
  }

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
};

template <typename edgeType>
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

  void addEdge(uint64_t from, uint64_t to) {
    if (from == to) return;
    mat[from][to] = true;
  }

  bool isConnected(uint64_t from, uint64_t to) const { return mat[from][to]; }

  void writedisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}
  void readdisk(const std::string&, std::shared_ptr<detra::IOAdapter>) {}

  void addVertices(uint64_t vertices) {
    for (auto& row : mat)
      row.resize(row.size() + vertices);

    mat.resize(mat.size() + vertices, std::vector<edgeType>(mat.size() + vertices));
  }
};

} // namespace backends

} // namespace graphs
