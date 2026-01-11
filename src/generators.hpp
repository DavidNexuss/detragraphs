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
 **/
template <typename GraphT, typename RandomSource = random_sources::XORand, bool directed = false>
GraphT erdos_renyi(uint64_t n, double p, RandomSource randomSource = RandomSource{}) {
  GraphT g;

  g.setType(directed ? GraphType::DIRECTED : GraphType::UNDIRECTED);
  g.addVertices(n);

  for (uint64_t i = 0; i < n; ++i) {
    for (uint64_t j = i + 1; j < n; ++j) {
      if (i != j && randomSource.randb()) {
        g.addEdge(j, i);
      }
    }
  }
  return g;
}

/**
 * Switch model generator
 * @param input The input graph
 * @param switches The number of switches
 * @param randomSource The random source (by default constructed specified in templated)
 * @returns The graph within the switch model generator
 **/
template <typename GraphInput, typename GraphOutput, typename RandomSource = random_sources::Standard>
GraphOutput switch_model(const GraphInput& input, int switches, RandomSource randomSource = RandomSource{}) {
  std::vector<std::pair<uint64_t, uint64_t>> edgelist = algorithms::to_edge_list(input);

  for (size_t i = 0; i < switches; i++) {
    std::pair<uint64_t, uint64_t> edgeSource;
    std::pair<uint64_t, uint64_t> edgeTarget;

    uint32_t edgeSourceIndex;
    uint32_t edgeTargetIndex;

    while (true) {
      edgeSourceIndex = randomSource.randi() % edgelist.size();
      edgeTargetIndex = randomSource.randi() % edgelist.size();

      edgeSource = edgelist[edgeSourceIndex];
      edgeTarget = edgelist[edgeTargetIndex];

      std::swap(edgeSource.second, edgeTarget.second);

      // Same edge
      if (edgeSource.first == edgeTarget.first && edgeSource.second && edgeTarget.second) continue;
      // Same endpoint
      if (edgeSource.first == edgeSource.second) continue;
      // Same endpoint
      if (edgeTarget.first == edgeTarget.second) continue;

      break;
    }

    edgelist[edgeSourceIndex] = edgeSource;
    edgelist[edgeTargetIndex] = edgeTarget;
  }
  return algorithms::from_edge_list<GraphOutput>(edgelist);
}


/**
 * Generate a Barabasi Albert graph with n vertices 
 **/
template <typename GraphT, typename RandomSource = random_sources::Standard, bool directed = false>
GraphT barabasi_albert(uint64_t n, uint64_t m0, uint64_t m, RandomSource randomSource = RandomSource{}) {
  if (m > m0 || m0 >= n) throw std::invalid_argument("Invalid parameters for BA model");

  GraphT g;
  g.addVertices(m0);
  g.setType(directed ? GraphType::DIRECTED : GraphType::UNDIRECTED);

  for (uint64_t i = 0; i < m0; ++i)
    for (uint64_t j = i + 1; j < m0; ++j)
      if (i != j) g.addEdge(j, i);

  std::vector<uint64_t> degreeList;
  for (uint64_t i = 0; i < m0; ++i)
    for (uint64_t d = 0; d < g.getVertexCount(); ++d)
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

/**
 * Returns a preferential directed generated graph using a given number of vertices and number of edges
 * @param n The nmber of vertices
 * @param e The number of edges
 * @param randomSource The randomSource used
 * @returns The graph
 **/
template <typename GraphT, typename RandomSource = random_sources::Standard>
GraphT preferential_directed(uint64_t n, uint64_t e, RandomSource randomSource = RandomSource{}) {
  GraphT g;
  g.addVertices(n);

  std::vector<uint64_t> preferentialNodes;
  preferentialNodes.reserve(2 * e);


  for (uint64_t i = 0; i < n; ++i) {
    preferentialNodes.push_back(i);
  }

  std::vector<uint64_t> inDegrees(n, 1);

  for (uint64_t i = 0; i < e; ++i) {
    uint64_t u = i % n;
    uint64_t v = u;

    size_t max_attempts = 10;
    while (u == v || g.isConnected(v, u)) {
      v = preferentialNodes[randomSource.randi() % preferentialNodes.size()];
      if (--max_attempts == 0) {
        v = n;
        break;
      }
    }

    if (v < n && u != v) {
      g.addEdge(v, u);
      preferentialNodes.push_back(v);
      inDegrees[v]++;
    }
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
  g.setType(GraphType::UNDIRECTED);

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
