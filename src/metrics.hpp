#pragma once
#include <vector>
#include <cstdint>
#include <numeric>
#include <cmath>
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

template <typename GraphT>
std::vector<uint32_t> degree_sequence_sorted(const GraphT& graph) {
  auto degrees = metrics::degree_sequence(graph);
  std::sort(degrees.begin(), degrees.end(), std::greater<>());
  return degrees;
}

template <typename GraphT>
std::vector<uint32_t> degree_sequence_sorted_log(const GraphT& graph) {
  auto degrees = metrics::degree_sequence_sorted(graph);
  for (uint32_t i = 0; i < degrees.size(); i++) degrees[i] = std::log(degrees[i]);
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
 * @brief Computes the shortest path distance matrix 
 * for a weighted, directed graph using the Floyd-Warshall algorithm.
 *
 * @param GraphT The graph type.
 * @param[in] graph The input graph (passed by reference) for which the distance matrix is computed.
 * @return A 2D vector of floats representing the distance matrix, where `dist[i][j]` is the shortest path distance.
 */
template <typename GraphT>
std::vector<std::vector<float>> distance_matrix(GraphT& graph) {
  uint64_t n = graph.getVertexCount();

  const float INF = std::numeric_limits<float>::infinity();

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
      if (std::isfinite(dist[i][k])) {
        for (uint64_t j = 0; j < n; ++j) {
          if (std::isfinite(dist[k][j])) {
            float new_dist = dist[i][k] + dist[k][j];
            dist[i][j]     = std::min(dist[i][j], new_dist);
          }
        }
      }
    }
  }

  return dist;
}

/**
 * @brief Computes the unweighted shortest path distance matrix
 * for a graph using BFS from each node. The distance is the minimum number of edges.
 *
 * @param GraphT The graph type.
 * @param[in] graph The input graph (passed by reference) for which the distance matrix is computed.
 * It is assumed GraphT has methods: getVertexCount(), getNeighbors(u), and an
 * implicit understanding that getNeighbors(u) returns the indices of nodes v
 * such that (u, v) is an edge.
 * @return A 2D vector of floats representing the distance matrix, where `dist[i][j]` is the
 * unweighted shortest path distance (number of edges). Unconnected nodes have distance INF.
 */
template <typename GraphT>
std::vector<std::vector<float>> distance_matrix_bfs(GraphT& graph) {
  uint64_t n = graph.getVertexCount();

  const float INF = std::numeric_limits<float>::infinity();

  std::vector<std::vector<float>> dist(n, std::vector<float>(n, INF));

  for (uint64_t s = 0; s < n; ++s) {
    std::vector<uint64_t> toVisit;
    std::vector<uint64_t> nextToVisit;

    uint32_t depth = 0;

    dist[s][s] = depth;
    toVisit.push_back(s);

    while (!toVisit.empty()) {
      depth++;

      for (uint64_t u : toVisit) {
        for (uint64_t v : graph.getEdges(u)) {
          if (dist[s][v] == INF) {
            dist[s][v] = depth;
            nextToVisit.push_back(v);
          }
        }
      }
      toVisit.clear();
      std::swap(nextToVisit, toVisit);
    }
  }

  return dist;
}

/**
 * @brief Returns the closeness centrality, which is the inverse of the sum of distances 
 * between a vertex and all other vertices in the network.
 * * Closeness C(v) = (N - 1) / Sum(d(v, u)) for all u != v.
 * Adheres to the standard definition: Closeness is 0 if the graph is not fully connected.
 * * @param GraphT The graph type.
 * @param[in] graph The input graph.
 * @param[in] source The vertex for which to calculate centrality.
 * @returns The closeness centrality score. Returns 0.0f if the source cannot reach all other nodes.
 */
template <typename GraphT>
float closeness_centrality(const GraphT& graph, uint64_t source) {
  std::vector<algorithms::WalkResult> distances = graphs::algorithms::walk_bfs(graph, source);

  double sum_of_distances = 0.0;

  uint64_t n = graph.getVertexCount();

  if (n <= 1) {
    return 0.0f;
  }

  if (distances.size() != n) {
    return 0.0f;
  }

  const float INF = std::numeric_limits<float>::infinity();

  for (const auto& result : distances) {
    float dist = (float)result.distanceFromSource;

    sum_of_distances += dist;
  }

  return (float)(((double)n - 1) / sum_of_distances);
}


/**
 * @brief Returns the closeness centrality for each vertex
 * Calls the corrected closeness_centrality function
 * @returns The closeness centrality vector
 */
template <typename GraphT>
std::vector<float> closeness(const GraphT& graph) {
  uint64_t           n = graph.getVertexCount();
  std::vector<float> results;
  results.reserve(n);

  for (uint64_t i = 0; i < n; i++)
    results.push_back(closeness_centrality(graph, i));

  return results;
}


/**
 * @brief Computes the closeness centrality for all vertices using a pre-computed 
 * All-Pairs Shortest Path (APSP) distance matrix, using the common formula for 
 * disconnected graphs: normalized by the number of reachable nodes.
 *
 * @param[in] dist_matrix A 2D vector where dist_matrix[i][j] is the shortest path 
 * distance from vertex i to vertex j.
 * @returns A vector of floats representing the closeness centrality score for each vertex.
 */
inline std::vector<float> closeness_matrix(const std::vector<std::vector<float>>& dist_matrix) {
  if (dist_matrix.empty()) {
    return {};
  }

  uint64_t           n = dist_matrix.size();
  std::vector<float> results;
  results.reserve(n);

  const float INF = std::numeric_limits<float>::infinity();

  for (uint64_t i = 0; i < n; i++) {
    double   sum_of_distances      = 0.0;
    uint64_t reachable_nodes_count = 0;

    for (uint64_t j = 0; j < n; j++) {
      if (i == j) {
        continue;
      }

      float dist = dist_matrix[i][j];

      if (dist < INF && dist >= 0) {
        sum_of_distances += dist;
        reachable_nodes_count++;
      }
    }

    if (reachable_nodes_count == (n - 1) && sum_of_distances > 0.0) {
      results.push_back((float)(((double)n - 1) / sum_of_distances));
    } else if (reachable_nodes_count < (n - 1) && sum_of_distances > 0.0) {
      double c_v = ((double)reachable_nodes_count) / sum_of_distances;

      double c_prime_v = c_v * ((double)reachable_nodes_count / (double)(n - 1));

      results.push_back(0.0f);
    } else {
      results.push_back(0.0f);
    }
  }

  return results;
}

} // namespace metrics
} // namespace graphs
