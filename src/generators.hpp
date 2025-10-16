#include "graph.hpp"
#include <random>

namespace graphs {
namespace generators {
template <typename GraphT>
GraphT erdos_renyi(uint64_t n, double p, uint64_t seed = std::random_device{}()) {
  GraphT g;
  if constexpr (requires(GraphT x) { x.resize(n); }) {
    g.resize(n);
  } else if constexpr (requires(GraphT x) { x.data.adj.resize(n); }) {
    g.data.adj.resize(n);
  } else if constexpr (requires(GraphT x) { x.adjacencyList.resize(n); }) {
    g.adjacencyList.resize(n);
  }

  std::mt19937_64             rng(seed);
  std::bernoulli_distribution dist(p);

  for (uint64_t i = 0; i < n; ++i) {
    for (uint64_t j = 0; j < n; ++j) {
      if (i != j && dist(rng)) {
        g.addEdge(i, j);
      }
    }
  }
  return g;
}

// 🌐 Barabási–Albert — Directed preferential attachment
template <typename GraphT>
GraphT barabasi_albert(uint64_t n, uint64_t m0, uint64_t m, uint64_t seed = std::random_device{}()) {
  if (m > m0 || m0 >= n) throw std::invalid_argument("Invalid parameters for BA model");

  GraphT g;
  if constexpr (requires(GraphT x) { x.resize(m0); }) {
    g.resize(m0);
  } else if constexpr (requires(GraphT x) { x.data.adj.resize(m0); }) {
    g.data.adj.resize(m0);
  } else if constexpr (requires(GraphT x) { x.adjacencyList.resize(m0); }) {
    g.adjacencyList.resize(m0);
  }

  std::mt19937_64 rng(seed);

  // Fully connected directed core
  for (uint64_t i = 0; i < m0; ++i)
    for (uint64_t j = 0; j < m0; ++j)
      if (i != j) g.addEdge(i, j);

  // Degree list
  std::vector<uint64_t> degreeList;
  for (uint64_t i = 0; i < m0; ++i)
    for (uint64_t d = 0; d < g.getVertexCount(); ++d)
      degreeList.push_back(i);

  // Extend to n vertices
  if constexpr (requires(GraphT x) { x.resize(n); }) {
    g.resize(n);
  } else if constexpr (requires(GraphT x) { x.data.adj.resize(n); }) {
    g.data.adj.resize(n);
  } else if constexpr (requires(GraphT x) { x.adjacencyList.resize(n); }) {
    g.adjacencyList.resize(n);
  }

  for (uint64_t i = m0; i < n; ++i) {
    std::unordered_set<uint64_t>            targets;
    std::uniform_int_distribution<uint64_t> pick(0, degreeList.size() - 1);

    while (targets.size() < m) {
      uint64_t chosen = degreeList[pick(rng)];
      if (chosen != i) targets.insert(chosen);
    }

    for (uint64_t t : targets) {
      g.addEdge(i, t);
      degreeList.push_back(t);
      degreeList.push_back(i);
    }
  }

  return g;
}

// 🔁 Watts–Strogatz — Directed small-world
template <typename GraphT>
GraphT watts_strogatz(uint64_t n, uint64_t k, double beta, uint64_t seed = std::random_device{}()) {
  if (k >= n) throw std::invalid_argument("k must be < n");

  GraphT g;
  if constexpr (requires(GraphT x) { x.resize(n); }) {
    g.resize(n);
  } else if constexpr (requires(GraphT x) { x.data.adj.resize(n); }) {
    g.data.adj.resize(n);
  } else if constexpr (requires(GraphT x) { x.adjacencyList.resize(n); }) {
    g.adjacencyList.resize(n);
  }

  std::mt19937_64                         rng(seed);
  std::uniform_real_distribution<double>  prob(0.0, 1.0);
  std::uniform_int_distribution<uint64_t> randNode(0, n - 1);

  // Create ring lattice (directed)
  for (uint64_t i = 0; i < n; ++i)
    for (uint64_t j = 1; j <= k; ++j)
      g.addEdge(i, (i + j) % n);

  // Rewire
  for (uint64_t i = 0; i < n; ++i) {
    for (uint64_t j = 1; j <= k; ++j) {
      uint64_t neighbor = (i + j) % n;
      if (prob(rng) < beta) {
        uint64_t newNeighbor;
        do {
          newNeighbor = randNode(rng);
        } while (newNeighbor == i || g.isConnected(i, newNeighbor));
        g.addEdge(i, newNeighbor);
      }
    }
  }

  return g;
}

} // namespace generators
} // namespace graphs
