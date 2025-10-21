#include <cstdint>
#include <unordered_set>
#include <detrarandom/random_sources.hpp>
#include <stdexcept>
#include <vector>
#include "graph.hpp"

namespace graphs {

namespace generators {

/**
 * @brief Generate an undiretced Erdos-Renyi graph using N vertices and connect them with probabiliy p
 * @param n N vertices
 * @param p probabiliy that two vertices are connected
 **/
template <typename GraphT, typename RandomSource, bool directed = false>
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

/*
template <typename GraphInput, typename GraphOutput, typename RandomSource>
GraphOutput switch_model(const GraphInput& input, int switches, RandomSource randomSource = RandomSource{}) {
  std::vector<std::pair<uint32_t, uint32_t>> edgelist = toedgelist(input);

  for (size_t i = 0; i < switches; i++) {
    std::pair<uint32_t, uint32_t> edgeSource;
    std::pair<uint32_t, uint32_t> edgeTarget;

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
  fromedgelist(edgelist);
}
*/

/**
 * Generate a Barabasi Albert graph with n vertices 
 **/
template <typename GraphT, typename RandomSource, bool directed = false>
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

template <typename GraphT, typename RandomSource, bool directed = false>
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

//Relaxed version of barabasi albert, faster to compute while retaining power scaling nature
//TODO: Prevent double edge addition
//TODO: Fix and correct

template <typename GraphT, typename RandomSource>
GraphT prefferential_directed(uint64_t n, uint64_t e, RandomSource randomSource = RandomSource{}) {
  GraphT g;
  g.addVertices(n);

  std::vector<uint64_t> preferentialNodes(n);
  for (int i = 0; i < preferentialNodes.size(); i++)
    preferentialNodes[i] = i;

  std::vector<uint64_t> degrees(n);

  for (int i = 0; i < e; i++) {
    int u = i % n;
    int v = u;

    while(u == v || g.isConnected(u, v)) 
      v = preferentialNodes[randomSource.randi() % preferentialNodes.size()];

    if (u != v) {
      g.addEdge(v, u);
      preferentialNodes.push_back(v);
      degrees[v]++;
    }
  }

  return g;
}

template <typename GraphT, typename RandomSource>
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

} // namespace generators
} // namespace graphs
