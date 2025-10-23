#pragma once
#include <vector>
#include <cstdint>
#include <thread>
#include <numeric>
#include "algorithms.hpp"

namespace graphs {
namespace metrics {

/**
 * Returns the degree sequence of the graph, that is, a vector where the ith value indicates the degree of the node i
 * @param graph The graph
 **/
template <typename GraphT>
std::vector<uint32_t> degree_sequence(const GraphT& graph) {
  int N = graph.getVertexCount();

  std::vector<uint32_t> degrees(N);

  for (int i = 0; i < N; i++) {
    degrees[i] = graph.getEdgeCount(i);
  }

  return degrees;
}

/**
 * Returns the average degree of a graph
 * @returns The average degree
 **/
template <typename GraphT>
double average_degree(const GraphT& graph) {
  auto   deg = degree_sequence(graph);
  double sum = std::accumulate(deg.begin(), deg.end(), 0.0);
  return deg.empty() ? 0.0 : sum / static_cast<double>(deg.size());
}

/**
 * Returns the density for agvien graph, that is 2E / (N * (N - 1))
 * @returns The density
 **/
template <typename GraphT>
double density(const GraphT& graph) {
  double N = static_cast<double>(graph.getVertexCount());
  double E = static_cast<double>(graph.getEdgeCount());
  if (N <= 1.0) return 0.0;
  return (2.0 * E) / (N * (N - 1.0));
}

/**
 * Computes the clustering coefficient for a gvien node.
 * That is
 * 
 * S <- neighbours(v)
 * result <- (sum i, j isConnected(i, j) for i <- S, j <- S ) / (|S| * (|S| - 1))
 * 
 * @returns result
 * 
 **/
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

/**
 * Computes the average clustering coefficient of the graph, that is the mean of all clustering coefficients
 * @returns The average clustering coefficient
 */
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

/**
 * Returns the closeness centrality, that is, the average of distances between a vertex and the other vertices of the network
 * Assumes the graph only has a one connected component
 * Uses the BFS algorithm
 * @returns The closeness
 */
template <typename GraphT>
float closeness_centrality(const GraphT& graph, uint64_t source) {
  std::vector<algorithms::WalkResult> distances = walk_bfs(graph, source);

  double average_distance = 0.0;

  for (size_t i = 0; i < distances.size(); i++)
    average_distance += distances[i].distanceFromSource;

  return average_distance / (graph.getVertexCount() - 1);
}

/**
 * @brief Computes the shortest path distance matrix 
 * for a weighted, directed graph using the Floyd-Warshall algorithm.
 *
 * @tparam GraphT The graph type.
 * @param[in] graph The input graph (passed by reference) for which the distance matrix is computed.
 * @return A 2D vector of floats representing the distance matrix, where `dist[i][j]` is the shortest path distance.
 */
template <typename GraphT>
std::vector<std::vector<float>> distance_matrix(GraphT& graph) {
  uint64_t n = graph.getVertexCount();

  const float                     INF = std::numeric_limits<float>::infinity();
  std::vector<std::vector<float>> dist(n, std::vector<float>(n, INF));

  for (uint64_t i = 0; i < n; ++i) {
    dist[i][i] = 0.0f;
  }

  for (uint64_t u = 0; u < n; ++u) {
    for (uint64_t v = 0; v < n; ++v) {
      if (graph.isConnected(u, v)) {
        dist[u][v] = graph.getEdgeWeight(u, v);
      }
    }
  }

  for (uint64_t k = 0; k < n; ++k) {
    for (uint64_t i = 0; i < n; ++i) {
      for (uint64_t j = 0; j < n; ++j) {
        if (dist[i][k] != INF && dist[k][j] != INF) {
          dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
        }
      }
    }
  }

  return dist;
}

/**
 * Returns the closeness centrality for each vertex
 * Calls the closeness_centrality function
 * @returns The closeness centrality vector
 */
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
