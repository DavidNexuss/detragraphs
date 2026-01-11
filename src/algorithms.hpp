#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>
#include <queue>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <assert.h>
#include <numeric>

namespace graphs {
namespace algorithms {

struct WalkResult {
  uint64_t vertexId;
  int      distanceFromSource;
};

template <typename GraphT>
void walk_dfs_recursive(const GraphT& graph, uint64_t u, std::vector<char>& vertex_visited, std::vector<WalkResult>& result, int depth) {
  for (uint64_t v : graph.getEdges(u)) {
    if (!vertex_visited[v]) {
      vertex_visited[v] = 1;
      result.push_back({v, depth});
      walk_dfs_recursive(graph, v, vertex_visited, result, depth + 1);
    }
  }
}
template <typename GraphT>
std::vector<WalkResult> walk_dfs(const GraphT& graph, uint64_t source) {
  int N = graph.getVertexCount();

  std::vector<char>       vertex_visited(N);
  std::vector<WalkResult> result;

  vertex_visited[source] = 1;

  walk_dfs_recursive(graph, source, vertex_visited, result, 1);
  return result;
}

template <typename GraphT>
std::vector<WalkResult> walk_bfs(const GraphT& graph, uint64_t source) {
  size_t N = graph.getVertexCount();

  std::vector<char>       vertex_visited(N, 0);
  std::vector<WalkResult> result;

  std::vector<uint64_t> toVisit;

  result.push_back({source, 0});
  vertex_visited[source] = 1;
  toVisit.push_back(source);

  int depth = 0;
  while (!toVisit.empty()) {
    depth++;
    std::vector<uint64_t> nextToVisit;

    for (uint64_t u : toVisit) {
      for (uint64_t v : graph.getEdges(u)) {
        if (!vertex_visited[v]) {
          vertex_visited[v] = 1;
          nextToVisit.push_back(v);
          result.push_back({v, depth});
        }
      }
    }

    toVisit.clear();
    std::swap(nextToVisit, toVisit);
  }
  return result;
}

/**
 * Returns BFS tree
 */
template <typename GraphT, typename GraphO>
GraphO walk_bfs_tree(const GraphT& graph, uint64_t source) {
  int    N = graph.getVertexCount();
  GraphO tree;
  tree.addVertices(N);

  std::vector<char>    visited(N);
  std::queue<uint64_t> q;

  visited[source] = 1;
  q.push(source);

  while (!q.empty()) {
    uint64_t u = q.front();
    q.pop();

    for (uint64_t v : graph.getEdges(u)) {
      if (!visited[v]) {
        visited[v] = 1;
        tree.addEdge(u, v);
        q.push(v);
      }
    }
  }
  return tree;
}
// === DFS TREE (returns GraphT) ===
template <typename GraphT, typename GraphO>
void dfs_tree_recursive(const GraphT& graph, GraphO& tree, uint64_t u, std::vector<char>& visited) {
  visited[u] = 1;
  for (uint64_t v : graph.getEdges(u)) {
    if (!visited[v]) {
      tree.addEdge(u, v);
      dfs_tree_recursive(graph, tree, v, visited);
    }
  }
}


/**
 * Performs a BFS walk around the graph
 */
template <typename GraphT, typename GraphO>
GraphO walk_dfs_tree(const GraphT& graph, uint64_t source) {
  int    N = graph.getVertexCount();
  GraphO tree;
  tree.addVertices(N);

  std::vector<char> visited(N);
  visited[source] = 1;
  dfs_tree_recursive(graph, tree, source, visited);
  return tree;
}

/**
 *  Converts a graph to an edge list
 */
template <typename GraphT>
std::vector<std::pair<uint64_t, uint64_t>> to_edge_list(GraphT& graph) {
  std::vector<std::pair<uint64_t, uint64_t>> edgelist;
  for (size_t i = 0; i < graph.getVertexCount(); i++) {
    for (uint64_t u : graph.getEdges(i)) edgelist.push_back({i, u});
  }
  return edgelist;
}

/**
 * Converts an edge list to a graph
 */
template <typename GraphT>
GraphT from_edge_list(const std::vector<std::pair<uint64_t, uint64_t>>& edges) {
  GraphT graph;

  if (edges.empty())
    return graph;

  uint64_t maxVertex = 0;
  for (auto& e : edges)
    maxVertex = std::max(maxVertex, std::max(e.first, e.second));

  graph.addVertices(maxVertex + 1);

  for (auto& [u, v] : edges) {
    graph.addEdge(u, v);
  }

  return graph;
}


/**
 * Runs prim algorithm and returns the tree in a edge list
 */
template <typename GraphT>
std::vector<std::pair<uint64_t, uint64_t>> prim(const GraphT& graph, uint64_t start = 0) {
  int N = graph.getVertexCount();

  std::vector<char>                          inMST(N, 0);
  std::vector<std::pair<uint64_t, uint64_t>> mstEdges;
  std::vector<double>                        minEdgeWeight(N, std::numeric_limits<double>::infinity());
  std::vector<int64_t>                       parent(N, -1);

  minEdgeWeight[start] = 0.0;

  for (int i = 0; i < N; ++i) {
    double minWeight = std::numeric_limits<double>::infinity();
    int    u         = -1;
    for (int v = 0; v < N; ++v) {
      if (!inMST[v] && minEdgeWeight[v] < minWeight) {
        minWeight = minEdgeWeight[v];
        u         = v;
      }
    }

    if (u == -1) break;
    inMST[u] = 1;

    if (parent[u] != -1)
      mstEdges.push_back({(uint64_t)parent[u], (uint64_t)u});

    for (uint64_t v : graph.getEdges(u)) {
      double w = graph.getEdgeWeight(u, v);
      if (!inMST[v] && w < minEdgeWeight[v]) {
        minEdgeWeight[v] = w;
        parent[v]        = u;
      }
    }
  }

  return mstEdges;
}

/**
 * Runs dijkstra algorithm
 */
template <typename GraphT>
std::vector<WalkResult> dijkstra(const GraphT& graph, uint64_t source) {
  int                     N = graph.getVertexCount();
  std::vector<double>     dist(N, std::numeric_limits<double>::infinity());
  std::vector<char>       visited(N);
  std::vector<WalkResult> result;

  using Pair = std::pair<double, uint64_t>;
  std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;

  dist[source] = 0.0;
  pq.push({0.0, source});

  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();
    if (visited[u]) continue;
    visited[u] = 1;
    result.push_back({u, static_cast<int>(d)});

    for (uint64_t v : graph.getEdges(u)) {
      double w = graph.getEdgeWeight(u, v);
      if (dist[v] > d + w) {
        dist[v] = d + w;
        pq.push({dist[v], v});
      }
    }
  }

  return result;
}
template <typename GraphT>
inline std::vector<std::vector<float>> computeDistanceMatrix(const GraphT& graph) {

  const float INF = std::numeric_limits<float>::infinity();

  int n = graph.getVertexCount();

  std::vector<std::vector<float>> dist(n, std::vector<float>(n, INF));

  for (size_t s = 0; s < n; ++s) {
    std::queue<size_t> q;
    dist[s][s] = 0.0f;
    q.push(s);
    while (!q.empty()) {
      size_t u = q.front();
      q.pop();
      for (size_t v = 0; v < n; ++v) {
        if (dist[s][v] == INF && graph.isConnectedUndirected(u, v)) {
          dist[s][v] = dist[s][u] + 1.0f;
          q.push(v);
        }
      }
    }
  }

  return dist;
}
/**
 * Generates randoms sample of N elements from a vector indexed from 0:M
 */
inline std::vector<uint64_t> random_sample(uint64_t N, uint64_t M) {
  if (N > M)
    throw std::invalid_argument("Sample size N cannot exceed range M.");

  std::vector<uint64_t> indices(M);
  std::iota(indices.begin(), indices.end(), 0);

  std::random_device rd;
  std::mt19937_64    gen(rd());
  std::shuffle(indices.begin(), indices.end(), gen);

  indices.resize(N);
  return indices;
}

// Generate a sampled subgraph containing only N vertices from the input graph
template <typename GraphT, typename GraphO>
GraphO sample_graph(const GraphT& graph, uint64_t N) {
  uint64_t M = graph.getVertexCount();
  if (N > M)
    throw std::invalid_argument("Sample size N cannot exceed vertex count.");

  std::vector<uint64_t>                  selected = random_sample(N, M);
  std::unordered_map<uint64_t, uint64_t> index_map;

  for (uint64_t i = 0; i < N; ++i)
    index_map[selected[i]] = i;

  GraphO out;
  out.addVertices(N);

  for (uint64_t old_u : selected) {
    for (uint64_t old_v : graph.getEdges(old_u)) {
      auto it = index_map.find(old_v);
      if (it != index_map.end()) {
        out.addEdge(index_map[old_u], it->second);
      }
    }
  }

  return out;
}
} // namespace algorithms
} // namespace graphs
