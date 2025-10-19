#include "generators.hpp"
#include "graphbackend/graphbackend.hpp"
#include "metrics.hpp"
#include "graph.hpp"
#include "printer.hpp"
#include "parser.hpp"
#include <chrono>
#include <iostream>
#include "algorithms.hpp"

using namespace graphs;
using namespace graphs::backends;

void benchmark() {
  using CGRaph = Graph<backends::AdjacencyMatrixFlat<char>>;

  constexpr size_t N  = 20;
  constexpr size_t m0 = 10;
  constexpr size_t m  = 3;

  CGRaph graph;

  {
    auto t0 = std::chrono::high_resolution_clock::now();

    graph = generators::erdos_renyi<CGRaph, random_sources::Standard>(200, 0.4);

    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Graph generated in "
              << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()
              << " microseconds\n";
  }

  {

    auto t0 = std::chrono::high_resolution_clock::now();

    uint64_t total_edges = graph.getEdgeCount();

    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Edge counting: " << total_edges
              << " edges in "
              << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()
              << " microseconds\n";
  }
  {
    auto t0 = std::chrono::high_resolution_clock::now();

    size_t connected_count = 0;
    for (size_t i = 0; i < N; ++i)
      for (size_t j = i + 1; j < N; ++j)
        connected_count += graph.isConnected(j, i);

    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Full connectivity check counted " << connected_count
              << " connections in "
              << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()
              << " microseconds\n";
  }
}

void test_prefferential() {
  printer::vector(metrics::degree_sequence(generators::prefferential_directed<backends::AdjacencyListVector, random_sources::XORand>(400, 9000)));
}

void test_tree() {
  printer::vector(metrics::degree_sequence(generators::recursive_tree<backends::AdjacencyListVector, random_sources::XORand>(7, 3, 0.9)));
}

void test_json_write() {
  using CGRaph = Graph<backends::AdjacencyMatrixFlat<char>>;
  CGRaph graph;

  graph.addVertices(32);
  graph.addEdge(3, 2);
  graph.addEdge(12, 1);
  graph.addEdge(24, 0);

  std::string code = parser::tojson(graph);

  using NGraph = Graph<backends::AdjacencyListVector>;

  Graph<AdjacencyListVector> g = parser::convert<Graph<AdjacencyMatrixFlat<char>>, Graph<AdjacencyListVector>>(graph);

  g.print();
}

void test_walk() {
  using G = Graph<backends::AdjacencyListVector>;

  std::cout << "\n=== WALK TESTS ===\n";

  // Generate a small connected graph
  G g = generators::erdos_renyi<G, random_sources::XORand>(40, 0.8);

  std::cout << "Original Graph:\n";
  g.print();

  auto dfs_walk = algorithms::walk_dfs(g, 0);
  std::cout << "\nDFS Walk (vertex, depth):\n";
  for (auto& r : dfs_walk)
    std::cout << "  (" << r.vertexId << ", depth=" << r.distanceFromSource << ")\n";

  auto bfs_walk = algorithms::walk_bfs(g, 0);
  std::cout << "\nBFS Walk (vertex, depth):\n";
  for (auto& r : bfs_walk)
    std::cout << "  (" << r.vertexId << ", depth=" << r.distanceFromSource << ")\n";

  auto bfs_tree = algorithms::walk_bfs_tree<G, G>(g, 0);
  std::cout << "\nBFS Tree:\n";
  bfs_tree.print();

  auto dfs_tree = algorithms::walk_dfs_tree<G, G>(g, 0);

  std::cout << "\nDFS Tree:\n";
  dfs_tree.print();

  // Metrics comparison: degree sequence difference
  auto deg_g     = metrics::degree_sequence(g);
  auto deg_dfs_t = metrics::degree_sequence(dfs_tree);
  auto deg_bfs_t = metrics::degree_sequence(bfs_tree);

  std::cout << "\nOriginal Degree Sequence:\n";
  printer::vector(deg_g);

  std::cout << "DFS Tree Degree Sequence:\n";
  printer::vector(deg_dfs_t);

  std::cout << "BFS Tree Degree Sequence:\n";
  printer::vector(deg_bfs_t);
}

void test_prim() {
  using G = Graph<backends::AdjacencyMatrixFlat<double>>;
  std::cout << "\n=== PRIM'S MST TEST ===\n";

  // Weighted connected graph
  G g = generators::erdos_renyi<G, random_sources::Standard>(10, 0.4);

  std::cout << "Original Weighted Graph:\n";
  g.print();

  auto mst = algorithms::from_edge_list<G>(algorithms::prim(g, 0));

  std::cout << "\nPrim's MST Graph:\n";

  // Compare edge count and approximate total weight
  double totalWeightOriginal = 0.0;
  double totalWeightMST      = 0.0;
  for (size_t u = 0; u < g.getVertexCount(); ++u)
    for (auto v : g.getEdges(u))
      totalWeightOriginal += 1;

  for (size_t u = 0; u < mst.getVertexCount(); ++u)
    for (auto v : mst.getEdges(u))
      totalWeightMST += 1;

  std::cout << "\nOriginal Total Weight: " << totalWeightOriginal / 2.0
            << "\nMST Total Weight:      " << totalWeightMST / 2.0
            << "\n";
}

void test_dijkstra() {
  using G = Graph<backends::AdjacencyMatrixFlat<double>>;
  std::cout << "\n=== DIJKSTRA TEST ===\n";

  G g = generators::erdos_renyi<G, random_sources::XORand>(10, 0.3);

  std::cout << "Weighted Graph:\n";
  g.print();

  auto results = algorithms::dijkstra(g, 0);
  std::cout << "\nDijkstra distances from source 0:\n";
  for (auto& r : results)
    std::cout << "  vertex " << r.vertexId << " -> distance " << r.distanceFromSource << "\n";
}

template <typename GraphType>
void test_clustering() {
  constexpr uint64_t TOTAL_VERTICES = 15000;
  constexpr double   EDGE_PROB      = 0.5;

  std::cout << "=== CLUSTERING COEFFICIENT TEST ===\n";

  auto t0 = std::chrono::high_resolution_clock::now();

  GraphType fullGraph = generators::erdos_renyi<GraphType, random_sources::XORand>(TOTAL_VERTICES, EDGE_PROB);

  auto t1 = std::chrono::high_resolution_clock::now();

  std::cout << "Generated graph of " << TOTAL_VERTICES << " vertices in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
            << " ms\n";

  // progressively sample graph and compute clustering
  std::cout << "SampleSize, ClusteringCoefficient\n";

  t0 = std::chrono::high_resolution_clock::now();

  double C = metrics::average_clustering_coefficient(fullGraph);

  t1 = std::chrono::high_resolution_clock::now();

  std::cout << "Computed all samples in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
            << " ms\n";
}

void test_eigensolver_complex() {}


int main() {
  return 0;
}
