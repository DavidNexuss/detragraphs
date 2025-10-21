#pragma once
#include <cstdint>
#include <vector>

namespace graphs {

enum class GraphType {
  DIRECTED,
  UNDIRECTED
};

template <typename Backend>
struct Graph {
  Backend   data;
  GraphType type = GraphType::DIRECTED;

  /**
   * Returns the number of vertices in the graph
   * @returns The number of vertices in the graph
   */
  inline uint64_t getVertexCount() const { return data.getVertexCount(); }

  /**
   * Returns the number of edges in the graph.
   * @returns The number of edges.
   */
  inline uint64_t getEdgeCount() const { return data.getEdgeCount(); }

  /**
   * Returns the number of edges in the graph for a particular vertex.
   * @returns The number of edges.
   */
  inline uint64_t getEdgeCount(uint64_t vertex) const { return data.getEdgeCount(vertex); }


  /**
   * Sets the type of the graph, UNDIRECTED or DIRECTED-
   */
  inline void setType(GraphType type) {
    this->type = type;
  }

  /**
   * Adds an adge to the graph.
   */
  inline void addEdge(uint64_t from, uint64_t to) {
    data.addEdge(from, to);
    if (type == GraphType::UNDIRECTED) {
      data.addEdge(to, from);
    }
  }

  /**
   * Checks if two nodes in the graph are connected.
   */
  inline bool isConnected(uint64_t from, uint64_t to) const { return data.isConnected(from, to); }

  /**
   * Checks if two nodes in the graph are connected.
   * Assumes underlying representation only has directed edges, in that case the order of edged checking is to < from, if
   * another order is provided the function will swap the values automatically.
   * @returns If its connected
   */
  inline bool isConnectedUndirected(uint64_t from, uint64_t to) const {
    if (to > from) std::swap(to, from);
    return data.isConnected(from, to);
  }

  /**
   * Adds N vertices to the graph
   **/
  inline void addVertices(uint64_t vertices) { data.addVertices(vertices); }

  /**
   * Returns the vertices that are adjacent to a particular vertex
   * @returns The list of vertices
   */
  inline std::vector<uint64_t> getEdges(uint64_t vertex) const { return data.getEdges(vertex); }

  /**
   * Returns a list of vertices that connect to a given a vertex.cA
   * @param vertex The vertex
   * @result The list of given vertices
   */
  inline std::vector<uint64_t> getInvertedEdges(uint64_t vertex) const { return data.getInvertedEdges(vertex); }

  inline void print() {
    data.print();
  }

  /**
   * Returns the weight of a particular connection of two vertices
   * @returns The weight
   */
  inline float getEdgeWeight(uint64_t u, uint64_t v) const {
    return 1.0f;
  }
};
} // namespace graphs
