#include "generators.hpp"
#include "graphbackend/graphbackend.hpp"
#include "metrics.hpp"
#include "graph.hpp"
#include "printer.hpp"
#include <chrono>
#include <iostream>

using namespace graphs;

void benchmark() {
  using CGRaph = Graph<backends::AdjacencyMatrixFlat<char>>;

  constexpr size_t N  = 20;
  constexpr size_t m0 = 10;
  constexpr size_t m  = 3;

  CGRaph graph;

  {
    auto t0 = std::chrono::high_resolution_clock::now();

    graph = generators::erdos_renyi_undirected<CGRaph, random_sources::Standard>(200, 0.4);

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

int main() {
  test_prefferential();
  test_tree();
  return 0;
