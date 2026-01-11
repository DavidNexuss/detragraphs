#pragma once
#include <vector>
#include <unordered_set>
#include <cstdint>
#include <algorithm>
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

  std::vector<uint64_t> getEdges(uint64_t u) const { return adj[u]; }

  void addEdge(uint64_t from, uint64_t to) {
    if (adj.size() <= from)
      adj.resize(from + 1);

    adj[from].push_back(to);
  }

  bool isConnected(uint64_t from, uint64_t to) const {
    return std::find(adj[from].begin(), adj[from].end(), to) != adj[from].end();
  }

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

  void addVertices(uint64_t vertices) {
    size_t old = offsets.size();
    offsets.resize(old + vertices, edges.size());
  }
  void print() {
  }
};

/**
 * @brief Represents a compressed, weighted directed graph using an Adjacency List.
 * * Each vertex stores a list of pairs (neighbor_index, weight).
 * This is an efficient, sparse representation suitable for weighted graphs.
 * * @tparam edgeType The type of the weight (e.g., float, double, int). Defaults to float.
 */
template <typename edgeType = float>
struct CompressedAdjacencyWeighted {
  // Data structure: Adjacency List storing pairs of (neighbor_index, weight)
  std::vector<std::vector<std::pair<uint64_t, edgeType>>> adj_list;

  /**
   * @brief Gets the number of vertices in the graph.
   * @return The number of vertices.
   */
  uint64_t getVertexCount() const { return adj_list.size(); }

  /**
   * @brief Gets the total number of edges in the graph.
   * @return The total edge count.
   */
  uint64_t getEdgeCount() const {
    uint64_t c = 0;
    for (const auto& row : adj_list)
      c += row.size();
    return c;
  }

  /**
   * @brief Gets the out-degree (number of edges) for a specific vertex.
   * @param vertex The index of the vertex.
   * @return The number of outgoing edges.
   */
  uint64_t getEdgeCount(uint64_t vertex) const {
    if (vertex >= adj_list.size()) return 0;
    return adj_list[vertex].size();
  }

  /**
   * @brief Gets the indices of all neighbors connected by outgoing edges.
   * @param vertex The index of the source vertex.
   * @return A vector of neighbor indices.
   */
  std::vector<uint64_t> getEdges(uint64_t vertex) const {
    std::vector<uint64_t> neighbours;
    if (vertex >= adj_list.size()) return neighbours;

    for (const auto& edge : adj_list[vertex])
      neighbours.push_back(edge.first);
    return neighbours;
  }

  /**
   * @brief Gets the indices and weights of all neighbors connected by outgoing edges.
   * @param vertex The index of the source vertex.
   * @return A vector of (neighbor_index, weight) pairs.
   */
  const std::vector<std::pair<uint64_t, edgeType>>& getEdgesWeighted(uint64_t vertex) const {
    // Return a reference to the internal list for efficiency
    static const std::vector<std::pair<uint64_t, edgeType>> empty_vec = {};
    if (vertex >= adj_list.size()) return empty_vec;
    return adj_list[vertex];
  }

  /**
   * @brief Gets the indices of vertices that have an edge pointing *to* the given vertex (in-edges).
   * @param vertex The index of the destination vertex.
   * @return A vector of source vertex indices.
   */
  std::vector<uint64_t> getInvertedEdges(uint64_t vertex) const {
    std::vector<uint64_t> result;
    // Note: This operation is O(V+E) for Adjacency List
    for (uint64_t i = 0; i < adj_list.size(); ++i) {
      for (const auto& edge : adj_list[i]) {
        if (edge.first == vertex) {
          result.push_back(i);
          break; // Found the edge, move to the next source vertex
        }
      }
    }
    return result;
  }

  /**
   * @brief Adds a directed, weighted edge to the graph. If the edge already exists, its weight is updated.
   * @param from The source vertex.
   * @param to The destination vertex.
   * @param weight The weight of the edge.
   */
  void addEdge(uint64_t from, uint64_t to, edgeType weight = edgeType{}) {
    if (from == to || from >= adj_list.size() || to >= adj_list.size()) return;

    // Check if edge already exists and update
    auto& neighbors = adj_list[from];
    for (auto& edge : neighbors) {
      if (edge.first == to) {
        edge.second = weight; // Update weight
        return;
      }
    }

    // Edge does not exist, add new one
    neighbors.emplace_back(to, weight);
  }

  /**
   * @brief Checks if a directed edge exists between two vertices.
   * @param from The source vertex.
   * @param to The destination vertex.
   * @return true if connected, false otherwise.
   */
  bool isConnected(uint64_t from, uint64_t to) const {
    if (from >= adj_list.size()) return false;
    // O(degree) lookup
    for (const auto& edge : adj_list[from]) {
      if (edge.first == to) return true;
    }
    return false;
  }

  /**
   * @brief Retrieves the weight of a directed edge.
   * @param from The source vertex.
   * @param to The destination vertex.
   * @return The weight of the edge, or edgeType{} if the edge does not exist.
   */
  edgeType getWeight(uint64_t from, uint64_t to) const {
    if (from >= adj_list.size()) return edgeType{};
    for (const auto& edge : adj_list[from]) {
      if (edge.first == to) return edge.second;
    }
    return edgeType{};
  }

  /**
   * @brief Adds a specified number of new, isolated vertices to the graph.
   * @param vertices The number of vertices to add.
   */
  void addVertices(uint64_t vertices) {
    adj_list.resize(adj_list.size() + vertices, {});
  }

  /**
   * @brief Prints the adjacency list representation of the graph.
   */
  void print() {
    std::cout << "---CompressedAdjacencyWeighted---" << std::endl;
    for (size_t i = 0; i < adj_list.size(); ++i) {
      std::cout << "V" << i << ": ";
      for (const auto& edge : adj_list[i]) {
        std::cout << "(" << edge.first << ", " << edge.second << ") ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  /**
  * @brief Computes and returns the inverse (transpose) graph.
  * The inverse graph contains the same vertices and edges,
  * but all edge directions are reversed. Weights are preserved.
  * @return A new CompressedAdjacencyWeighted instance representing the inverse graph.
  */
  CompressedAdjacencyWeighted<edgeType> getInverseGraph() const {
    CompressedAdjacencyWeighted<edgeType> inverse_graph;
    uint64_t                              num_vertices = getVertexCount();
    inverse_graph.addVertices(num_vertices);

    for (uint64_t from = 0; from < num_vertices; ++from) {
      for (const auto& edge : adj_list[from]) {
        uint64_t to     = edge.first;
        edgeType weight = edge.second;
        inverse_graph.addEdge(to, from, weight);
      }
    }

    return inverse_graph;
  }
};

} // namespace backends

} // namespace graphs
