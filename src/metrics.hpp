#pragma once
#include <vector>
#include <cstdint>
#include <queue>
#include <numeric>
#include "algorithms.hpp"

namespace graphs {

namespace metrics {

template <typename GraphT>
std::vector<uint32_t> degree_sequence(const GraphT& graph) {
  int N = graph.getVertexCount();

  std::vector<uint32_t> degrees(N);

  for (int i = 0; i < N; i++) {
    degrees[i] = graph.getEdgeCount(i);
  }

  return degrees;
}

template <typename GraphT>
double average_degree(const GraphT& graph) {
  auto   deg = degree_sequence(graph);
  double sum = std::accumulate(deg.begin(), deg.end(), 0.0);
  return deg.empty() ? 0.0 : sum / static_cast<double>(deg.size());
}

template <typename GraphT>
double density(const GraphT& graph) {
  double N = static_cast<double>(graph.getVertexCount());
  double E = static_cast<double>(graph.getEdgeCount());
  if (N <= 1.0) return 0.0;
  return (2.0 * E) / (N * (N - 1.0));
}

template <typename GraphT>
uint32_t connected_components(const GraphT& graph) {
  int               N = graph.getVertexCount();
  std::vector<bool> visited(N, false);
  uint32_t          components = 0;
  std::queue<int>   q;

  for (int i = 0; i < N; i++) {
    if (visited[i]) continue;
    components++;
    q.push(i);
    visited[i] = true;

    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (auto v : graph.getEdges(u)) {
        if (!visited[v]) {
          visited[v] = true;
          q.push(v);
        }
      }
    }
  }

  return components;
}

template <typename GraphT>
double average_shortest_path_length(const GraphT& graph) {
  int N = graph.getVertexCount();
  if (N <= 1) return 0.0;

  double   total = 0.0;
  uint64_t pairs = 0;

  for (int src = 0; src < N; src++) {
    std::vector<int> dist(N, -1);
    std::queue<int>  q;
    dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (auto v : graph.getEdges(u)) {
        if (dist[v] == -1) {
          dist[v] = dist[u] + 1;
          q.push(v);
        }
      }
    }

    for (int i = 0; i < N; i++) {
      if (i != src && dist[i] > 0) {
        total += dist[i];
        pairs++;
      }
    }
  }

  return pairs > 0 ? total / pairs : 0.0;
}

template <typename GraphT>
uint32_t diameter(const GraphT& graph) {
  int      N       = graph.getVertexCount();
  uint32_t maxDist = 0;

  for (int src = 0; src < N; src++) {
    std::vector<int> dist(N, -1);
    std::queue<int>  q;
    dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (auto v : graph.getEdges(u)) {
        if (dist[v] == -1) {
          dist[v] = dist[u] + 1;
          q.push(v);
        }
      }
    }

    for (int d : dist)
      if (d > maxDist) maxDist = d;
  }

  return maxDist;
}

template <typename GraphT>
double clustering_coefficient(const GraphT& graph, uint64_t v) {
  auto neighbors = graph.getEdges(v);
  int  k         = static_cast<int>(neighbors.size());
  if (k < 2) return 0.0;

  int links = 0;
  for (int i = 0; i < k; ++i)
    for (int j = i + 1; j < k; ++j)
      if (graph.isConnected(neighbors[i], neighbors[j]))
        links++;

  return (2.0 * links) / (k * (k - 1));
}

template <typename GraphT>
double average_clustering_coefficient(const GraphT& graph) {
  int N = graph.getVertexCount();
  if (N == 0) return 0.0;
  double total = 0.0;

  for (int i = 0; i < N; i++) {
    total += clustering_coefficient(graph, i);
  }

  return total / N;
}

template <typename GraphT>
float closeness_centrality(const GraphT& graph, uint64_t source) {
  std::vector<algorithms::WalkResult> distances = walk_bfs(graph, source);

  double average_distance = 0.0;

  for (size_t i = 0; i < distances.size(); i++)
    average_distance += distances[i].distanceFromSource;

  return average_distance / (graph.getVertexCount() - 1);
}

template <typename GraphT>
std::vector<float> closeness(const GraphT& graph) {
  std::vector<float> results;

  struct Result {
    std::vector<float> results;
  } __attribute__((packed, aligned(64)));

  static_assert((sizeof(Result) % 64) == 0);

  alignas(64) Result partialResults[std::thread::hardware_concurrency()];

#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < graph.getVertexCount(); i++)
    partialResults[omp_get_thread_num()].results.push_back(closeness_centrality(graph, i));

  for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
    for (auto val : partialResults[i].results) results.push_back(val);
  }

  return results;
}

} // namespace metrics
} // namespace graphs
