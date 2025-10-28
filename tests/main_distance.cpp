#include <iostream>
#include <vector>
#include <cmath>
#include <metrics.hpp>
#include <graph.hpp>
#include <generators.hpp>
#include <graphbackend/adjacencymatrix.hpp>
#include <iomanip>

using namespace graphs;
using namespace graphs::metrics;
using namespace graphs::generators;

using MyGraph = graphs::Graph<backends::AdjacencyMatrixFlat<char>>;

bool compare_matrices(const std::vector<std::vector<float>>& m1, const std::vector<std::vector<float>>& m2, float tolerance = 1e-6) {
  if (m1.size() != m2.size()) return false;
  for (size_t i = 0; i < m1.size(); ++i) {
    if (m1[i].size() != m2[i].size()) return false;
    for (size_t j = 0; j < m1[i].size(); ++j) {
      if (std::isinf(m1[i][j]) && std::isinf(m2[i][j])) continue;
      if (std::isinf(m1[i][j]) != std::isinf(m2[i][j])) return false;
      if (std::abs(m1[i][j] - m2[i][j]) > tolerance) return false;
    }
  }
  return true;
}

void print_matrix(const std::vector<std::vector<float>>& matrix) {
  for (const auto& row : matrix) {
    for (float val : row) {
      if (std::isinf(val)) {
        std::cout << std::setw(6) << "INF";
      } else {
        // Print integers neatly
        std::cout << std::setw(6) << (int)val;
      }
    }
    std::cout << "\n";
  }
}

bool compare_closeness_scores(const std::vector<float>& c1, const std::vector<float>& c2, float tolerance = 1e-6) {
  if (c1.size() != c2.size()) return false;
  for (size_t i = 0; i < c1.size(); ++i) {
    if (std::abs(c1[i] - c2[i]) > tolerance) {
      std::cout << "Mismatch at vertex " << i << ": BFS=" << c1[i] << ", Matrix=" << c2[i] << "\n";
      return false;
    }
  }
  return true;
}

void print_closeness_scores(const std::vector<float>& scores, const std::string& label) {
  std::cout << "--- " << label << " ---\n";
  for (size_t i = 0; i < scores.size(); ++i) {
    std::cout << "Vertex " << i << ": " << std::fixed << std::setprecision(5) << scores[i] << "\n";
  }
}


int main() {
  std::cout << "Starting distance matrix test...\n";

  MyGraph graph = generators::erdos_renyi<MyGraph>(5, 0.1);
  graph.print();

  std::cout << "Graph structure (unweighted edges):\n";

  std::vector<std::vector<float>> dm_fw = distance_matrix(graph);
  std::cout << "\n--- Floyd-Warshall Distance Matrix ---\n";

  std::vector<std::vector<float>> dm_bfs = distance_matrix_bfs(graph);
  std::cout << "\n--- All-Pairs BFS Distance Matrix ---\n";

  std::cout << "\n--- All-Pairs BFS Distance Matrix ---\n";
  print_matrix(dm_bfs);
  std::cout << "\n--- Floyd-Warshall Distance Matrix ---\n";
  print_matrix(dm_fw);
  std::cout << "\n--- Comparison Result ---\n";
  if (compare_matrices(dm_fw, dm_bfs)) {
    std::cout << "✅ SUCCESS: The distance matrices from Floyd-Warshall (weighted with 1.0) and BFS (unweighted) are identical.\n";
  } else {
    std::cout << "❌ FAILURE: The distance matrices are different.\n";
  }

  std::vector<float> closeness_bfs = metrics::closeness(graph);

  std::vector<float> closeness_matrix_based = metrics::closeness_matrix(dm_fw);

  std::cout << "\n--- Closeness Centrality Results ---\n";

  print_closeness_scores(closeness_bfs, "Multiple BFS Walks (Reference)");
  print_closeness_scores(closeness_matrix_based, "Distance Matrix Based (Test)");

  std::cout << "\n--- Comparison Result ---\n";
  if (compare_closeness_scores(closeness_bfs, closeness_matrix_based)) {
    std::cout << "✅ SUCCESS: Closeness Centrality scores derived from multiple BFS walks and the distance matrix are identical.\n";
  } else {
    std::cout << "❌ FAILURE: Closeness Centrality scores differ between the two methods.\n";
  }
}
