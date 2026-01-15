#pragma once
#include <cstdint>
#include <unordered_set>
#include <detrarandom/random_sources.hpp>
#include <stdexcept>
#include <vector>
#include "algorithms.hpp"
#include "graph.hpp"

namespace graphs {

/**
 * @brief Graph generation algorithms for creating various random graph models.
 * This file contains implementations of random graph generation algorithms,
 * including
 * Erdos-Renyi, Barabasi-Albert, Watts-Strogatz, Preferential Directed, and
 * Recursive Tree models.
 * All generators are templated to support different graph types and random
 *number sources.
 **/
namespace generators {

/**
 * @brief Generate an undiretced Erdos-Renyi graph using N vertices and connect them with probabiliy p
 * @param n N vertices
 * @param p probabiliy that two vertices are connected
 * @param randomSource the random source used
 **/
template <typename GraphT, typename RandomSource = random_sources::XORand, bool directed = false>
GraphT erdos_renyi(uint64_t n, double p, RandomSource randomSource = RandomSource{}) {
  GraphT g;

  g.setType(directed ? GraphType::DIRECTED : GraphType::UNDIRECTED);
  g.addVertices(n);

  for (uint64_t i = 0; i < n; ++i) {
    uint64_t start_j = directed ? 0 : i + 1;
    for (uint64_t j = start_j; j < n; ++j) {
      if (i != j && randomSource.randf() < p) {
        g.addEdge(i, j);
      }
    }
  }
  return g;
}

template <typename GraphInput, typename GraphOutput, typename RandomSource = random_sources::Standard>
GraphOutput switch_model(const GraphInput& input, int switches, RandomSource randomSource = RandomSource{}) {
  std::vector<std::pair<uint64_t, uint64_t>> edgelist = algorithms::to_edge_list(input);

  for (size_t i = 0; i < switches; i++) {
    uint64_t u, v, x, y; // u-v and x-y are the existing edges
    uint32_t edgeSourceIndex;
    uint32_t edgeTargetIndex;

    while (true) {
      edgeSourceIndex = randomSource.randi() % edgelist.size();
      do {
        edgeTargetIndex = randomSource.randi() % edgelist.size();
      } while (edgeSourceIndex == edgeTargetIndex);

      u = edgelist[edgeSourceIndex].first;
      v = edgelist[edgeSourceIndex].second;
      x = edgelist[edgeTargetIndex].first;
      y = edgelist[edgeTargetIndex].second;

      if (u == x || u == y || v == x || v == y) continue;

      if (u == y || x == v) continue;

      break;
    }

    edgelist[edgeSourceIndex] = {u, y}; // u-v becomes u-y
    edgelist[edgeTargetIndex] = {x, v}; // x-y becomes x-v
  }
  return algorithms::from_edge_list<GraphOutput>(edgelist);
}


template <typename GraphT, typename RandomSource = random_sources::Standard, bool directed = false>
GraphT barabasi_albert(uint64_t n, uint64_t m0, uint64_t m, RandomSource randomSource = RandomSource{}) {
  if (m > m0 || m0 >= n) throw std::invalid_argument("Invalid parameters for BA model");

  GraphT g;
  g.addVertices(m0);
  g.setType(directed ? GraphType::DIRECTED : GraphType::UNDIRECTED);

  for (uint64_t i = 0; i < m0; ++i)
    for (uint64_t j = i + 1; j < m0; ++j)
      g.addEdge(i, j);

  std::vector<uint64_t> degreeList;
  for (uint64_t i = 0; i < m0; ++i)
    for (uint64_t d = 0; d < m0 - 1; ++d)
      degreeList.push_back(i);

  g.addVertices(n - m0);

  for (uint64_t i = m0; i < n; ++i) {
    std::unordered_set<uint64_t> targets;

    while (targets.size() < m) {
      uint64_t chosen = degreeList[randomSource.randi() % (degreeList.size())];
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

template <typename GraphT, typename RandomSource = random_sources::Standard, bool directed = false>
GraphT watts_strogatz(uint64_t n, uint64_t k, double beta, RandomSource randomSource = RandomSource{}) {
  if (k >= n) throw std::invalid_argument("k must be < n");

  GraphT g;
  g.setType(directed ? GraphType::DIRECTED : GraphType::UNDIRECTED);
  g.addVertices(n);

  for (uint64_t i = 0; i < n; ++i)
    for (uint64_t j = 1; j <= k; ++j)
      g.addEdge(i, (i + j) % n);

  for (uint64_t i = 0; i < n; ++i) {
    for (uint64_t j = 1; j <= k; ++j) {
      uint64_t neighbor = (i + j) % n;

      if (randomSource.randf() < beta) {
        uint64_t newNeighbor;
        do {
          newNeighbor = randomSource.randi() % n;
        } while (newNeighbor == i || g.isConnected(i, newNeighbor));

        g.addEdge(i, newNeighbor);
      }
    }
  }

  return g;
}

template <typename GraphT, typename RandomSource = random_sources::Standard>
GraphT preferential_directed(uint64_t n, uint64_t e, RandomSource randomSource = RandomSource{}) {
  GraphT g;
  g.addVertices(n);
  g.setType(GraphType::DIRECTED); // Assuming directed is intended

  std::vector<uint64_t> preferentialNodes;
  preferentialNodes.reserve(2 * e);

  for (uint64_t i = 0; i < n; ++i) {
    preferentialNodes.push_back(i);
  }


  for (uint64_t i = 0; i < e; ++i) {
    uint64_t u = randomSource.randi() % n;

    uint64_t v;
    size_t   max_attempts = 100;

    while (true) {
      v = preferentialNodes[randomSource.randi() % preferentialNodes.size()];

      if (u != v && !g.isConnected(u, v)) {
        break;
      }

      if (--max_attempts == 0) {
        goto next_edge;
      }
    }

    g.addEdge(u, v);
    preferentialNodes.push_back(v);

  next_edge:; // Label for `goto` to skip edge addition
  }

  return g;
}


template <typename GraphT>
GraphT factor_graph_fast_seed(uint64_t N, uint64_t gamma, float seed) {
  GraphT g;
  g.setType(GraphType::UNDIRECTED);

  auto hash = [N, seed](uint64_t x) {
    return (x * 137 + uint64_t(871 * seed)) % N;
  };

  g.addVertices(N);

  for (uint64_t u = 2; u < N + 2; ++u) {
    uint64_t offset = std::pow(u, gamma);
    for (uint64_t v = u; v < N + 2; v += offset) {
      g.addEdge(u - 2, hash(v - 2));
    }
  }

  return g;
}

template <typename GraphT>
GraphT factor_graph_fast(uint64_t N, uint64_t gamma) {
  GraphT g;
  g.setType(GraphType::UNDIRECTED);

  g.addVertices(N);

  for (uint64_t u = 2; u < N + 2; ++u) {
    uint64_t offset = std::pow(u, gamma);
    for (uint64_t v = u; v < N + 2; v += offset)
      g.addEdge(u - 2, v - 2);
  }

  return g;
}

/**
 * Generates a recursive tree
 * @param levels The number of levels of the tree
 * @param maxlevelcount The maximum number of vertices in each level
 * @param p The probability to spawn a node in a given levle
 * @returns The graph
 */
template <typename GraphT, typename RandomSource = random_sources::Standard>
GraphT recursive_tree(uint64_t levels, uint64_t maxlevelcount, float p, RandomSource randomSource = RandomSource{}) {
  GraphT g;

  if (levels == 0) return g;

  g.addVertices(1);

  std::vector<uint64_t> currentLevel = {0};
  uint64_t              nextVertex   = 1;

  for (uint64_t level = 1; level < levels; ++level) {
    std::vector<uint64_t> nextLevel;

    for (uint64_t parent : currentLevel) {
      for (uint64_t i = 0; i < maxlevelcount; ++i) {
        float r = randomSource.randi() / float(std::numeric_limits<uint32_t>::max());
        if (r < p) {
          g.addVertices(1);
          g.addEdge(parent, nextVertex);
          nextLevel.push_back(nextVertex);
          ++nextVertex;
        }
      }
    }

    if (nextLevel.empty()) break;
    currentLevel = std::move(nextLevel);
  }

  return g;
}

template <typename GraphT>
GraphT generate() {}

} // namespace generators
} // namespace graphs
