#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <metrics.hpp>
#include <graph.hpp>
#include <generators.hpp>
#include <graphbackend/adjacencymatrix.hpp>
#include <util/vector_utils.hpp>

using namespace graphs;
using namespace graphs::metrics;
using namespace graphs::generators;

using MyGraph = graphs::Graph<backends::AdjacencyMatrixFlat<char>>;

// --- Utility Functions ---

/**
 * @brief Calculates the average Closeness Centrality for a graph.
 * @param scores Vector of CC scores for each vertex.
 * @return The average CC.
 */
float calculate_average_closeness(const std::vector<float>& scores) {
  if (scores.empty()) return 0.0f;
  std::cout << scores << std::endl;
  float sum = std::accumulate(scores.begin(), scores.end(), 0.0f);
  return sum / scores.size();
}

/**
 * @brief Generates a Star Graph (one central node, N-1 leaves).
 */
MyGraph generate_star_graph(uint64_t n) {
  MyGraph g;
  g.setType(GraphType::UNDIRECTED);
  g.addVertices(n);
  // Vertex 0 is the center
  for (uint64_t i = 1; i < n; ++i) {
    g.addEdge(0, i);
  }
  return g;
}

/**
 * @brief Generates a Line Graph (Path Graph).
 */
MyGraph generate_line_graph(uint64_t n) {
  MyGraph g;
  g.setType(GraphType::UNDIRECTED);
  g.addVertices(n);
  for (uint64_t i = 0; i < n - 1; ++i) {
    g.addEdge(i, i + 1);
  }
  return g;
}

/**
 * @brief Runs a benchmark for a specific graph type and compares its average CC
 * to a theoretical expectation.
 * @param g The generated graph.
 * @param theoretical_cc The expected or reference CC value.
 * @param name Name of the graph model.
 * @param print_g If true, prints the graph structure (only for N <= 25).
 */
void run_benchmark(MyGraph& g, float theoretical_cc, const std::string& name, bool print_g) {
  const uint64_t N = g.getVertexCount();

  // Print structure for small graphs
  if (print_g && N <= 25) {
    std::cout << "\n--- Graph Structure: " << name << " (N=" << N << ") ---\n";
    g.print();
    std::cout << "------------------------------------------------------\n";
  }

  // 1. Calculate actual CC scores
  std::vector<float> closeness_scores = metrics::closeness(g);
  float              actual_avg_cc    = calculate_average_closeness(closeness_scores);

  // 2. Calculate observed Average Shortest Path Length (L_bar)
  // Formula: L̄ ≈ (N-1) / (Avg CC * N)
  float observed_L_bar = 0.0f;
  if (actual_avg_cc > 1e-6) {
    observed_L_bar = (N - 1.0f) / (actual_avg_cc * (N));
  }

  std::cout << "--- " << name << " Benchmark (N=" << N << ") ---\n";
  std::cout << std::fixed << std::setprecision(4);

  std::cout << "Observed Avg Closeness (CC): " << actual_avg_cc;
  std::cout << " (Expected/Target: " << theoretical_cc << ")\n";

  std::cout << "Observed Avg Path Length (L̄): " << observed_L_bar << "\n";

  // Simple check to see if the observed value is reasonably close to the theoretical
  if (theoretical_cc > 0.0f && std::abs(actual_avg_cc - theoretical_cc) < 0.05 * theoretical_cc) {
    std::cout << "✅ Result is close to theoretical expectation.\n";
  } else {
    std::cout << "⚠️ Result deviates significantly from expectation.\n";
  }
}

// --- Main Execution ---

int main() {
  std::cout << "--- Focused Closeness Centrality Benchmark ---\n";
  std::cout << "Testing network efficiency (Closeness) on various structures.\n";
  std::cout << "Note: Theoretical CC for random graphs is an approximation (1/L̄).\n";

  // --- 1. Line Graph (Baseline - Low Closeness) ---
  const uint64_t N_LINE              = 10;
  MyGraph        line_graph          = generate_line_graph(N_LINE);
  float          theoretical_line_cc = 0.17f; // Approx. avg for N=10
  std::cout << "\n======================================================\n";
  run_benchmark(line_graph, theoretical_line_cc, "Line Graph (P_10, Max Distance)", true);

  // --- 2. Star Graph (Baseline - Max Tree Closeness) ---
  const uint64_t N_STAR              = 10;
  MyGraph        star_graph          = generate_star_graph(N_STAR);
  float          theoretical_star_cc = 0.526f; // Exact value for N=10
  std::cout << "\n======================================================\n";
  run_benchmark(star_graph, theoretical_star_cc, "Star Graph (Maximum Tree Closeness)", true);

  // --------------------------------------------------------------------------
  // --- FOCUS: Erdos-Renyi Graph (Small-World/Random Graph) ---
  // N=20, P=0.2. Np=4. Expected L̄ ~ log(20)/log(4) ~ 2.16. CC ~ 1/2.16 ~ 0.46
  const uint64_t N_ER     = 20;
  const double   P_ER     = 0.2;
  MyGraph        er_graph = generators::erdos_renyi<MyGraph>(N_ER, P_ER);

  // Theoretical L̄ calculation
  float theoretical_er_L_bar = std::log(N_ER) / std::log(N_ER * P_ER);
  float theoretical_er_cc    = 1.0f / theoretical_er_L_bar;

  std::cout << "\n======================================================\n";
  run_benchmark(er_graph, theoretical_er_cc, "Erdos-Renyi G(N,p) (Random Shortcuts)", true);

  // --------------------------------------------------------------------------
  // --- FOCUS: Recursive Tree (Tree Structure, Low CC Expected) ---
  // Levels=4, MaxNodesPerLevel=2, Prob=0.7. Maximum N = 15.
  const uint64_t TREE_LEVELS        = 4;
  const uint64_t TREE_MAX_PER_LEVEL = 2;
  const float    TREE_P             = 0.7f;

  MyGraph tree_g = generators::recursive_tree<MyGraph>(TREE_LEVELS, TREE_MAX_PER_LEVEL, TREE_P);
  tree_g.setType(GraphType::UNDIRECTED);

  // We expect the tree to be less centralized than a star, so CC should be lower than 0.5.
  // Setting a theoretical target based on a typical tree's low CC.
  float expected_tree_cc_low = 0.25f;

  std::cout << "\n======================================================\n";
  run_benchmark(tree_g, expected_tree_cc_low, "Recursive Tree (Intermediate Tree)", true);

  // --- Watts-Strogatz Graph (Included but reduced in size) ---
  const uint64_t N_WS     = 20;
  const uint64_t K_WS     = 4;
  const double   BETA_WS  = 0.1;
  MyGraph        ws_graph = generators::watts_strogatz<MyGraph>(N_WS, K_WS, BETA_WS);
  std::cout << "\n======================================================\n";
  run_benchmark(ws_graph, theoretical_er_cc, "Watts-Strogatz (Small-World Target)", false);

  std::cout << "\n======================================================\n";

  return 0;
}
