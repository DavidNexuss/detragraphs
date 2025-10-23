# Detragraphs

**Detragraphs** is a templated C++ library for **procedural graph generation and manipulation**, forming part of the **Detramotor Core** ecosystem.  
It provides a collection of foundational algorithms for creating **random, structured, and evolving graph models** — useful for simulation, procedural world generation, network modeling, and algorithmic research.

---

## 🌐 Overview

Detragraphs is designed to be:
- **Header-only** and **templated** for integration with custom graph structures.  
- Compatible with any **random number source** (DetraRandom or user-defined).  
- **Deterministic**, **lightweight**, and **composable**.  
- Extendable through modular graph algorithms.

# 🚀 Features

- Graph Analysis: Implementations of fundamental graph algorithms (Shortest Path, Centrality).
- Sparse Linear Algebra: Efficient representation of sparse matrices (CSR format) and specialized matrix-vector multiplication.
- Iterative Solvers: Jacobi's method for finding dominant eigenvectors (useful for ranking/PageRank-like problems).
- Statistical Utilities: Basic vector-based statistics (Mean, Standard Deviation, Dot Product).
- JSON Integration: Tools for parsing and serializing graphs to and from JSON format using nlohmann/json.

# 🛠️ Requirements

This library is designed to be header-only.

C++17 or later

External Dependencies:

- glm: Required for graph position handling (graphs::position::PositionTable).
- nlohmann/json: Required for all graph parsing and encoding utilities (graphs::parser).

# 📊 Core Functionality & Usage

## Graph Algorithms

The library provides templated functions that operate on any custom graph type (GraphT) that satisfies a basic interface (e.g., isConnected(u, v), getEdgeWeight(u, v)).



| Function | Description | Example Input |
| :--- | :--- | :--- |
| `floyd_warshall(graph)` | Computes all-pairs shortest paths in a graph. | `std::vector<std::vector<float>> distances = metrics::distance_matrix(my_graph);` |
| `closeness(graph)` | Calculates the closeness centrality for every vertex. | `std::vector<float> centrality = metrics::closeness(my_graph);` |

### Example: Calculating All-Pairs Shortest Paths

```c++
#include <detragraphs/graph.hpp>
#include <detragraphs/graphbackend/adjacencymatrix.hpp>

using namespace graphs;

using MyGraph = Graph<backends::AdjacencyMatrix<char>>;

void analyze_graph(const MyGraph& my_graph) {
    std::vector<std::vector<float>> shortest_distances = metrics::distance_matrix(my_graph);

    std::cout << "Distance 0 -> 5: " << shortest_distances[0][5] << std::endl;
}
```


## Iterative Solvers (Jacobi/Power Iteration)

The jacobi template function finds the dominant eigenvector of a square matrix $\mathbf{A}$ by repeatedly applying the matrix-vector multiplication $\mathbf{x}_{k+1} = \mathbf{A} \mathbf{x}_k$ until convergence.

The matrix type used (e.g., CSRMatrix, DenseMatrix) must expose:

```c++
using Vector = std::vector<float>.
int N, int M; // (dimensions).
void apply_inplace(const Vector& input, Vector& output).
```

```c++
#include <detragraphs/math/multiply.hpp>
#include <detragraphs/math/csrmatrix.hpp> // Or your DenseMatrix implementation

void solve_pagerank_like() {
    // Assume a CSRMatrix A (N x N) has been initialized
    CSRMatrix A = ...; 
    
    // Calculate the dominant eigenvector
    std::vector<float> eigenvector = detra::math::jacobi(A);

    // The result vector holds the final, converged eigenvector.
    std::cout << "Solved Vector Size: " << eigenvector.size() << std::endl;
}
```

## Statistical Utilities

Basic functions for vector analysis are available in the graphs::stats namespace.

```c++
#include <graphs/stats.hpp>
#include <vector
#include <iostream>

void analyze_data() {
    std::vector<float> data = {10.0f, 15.0f, 20.0f, 25.0f};
    std::vector<float> target = {9.9f, 15.1f, 20.0f, 25.0f};

    // Statistical functions
    std::cout << "Mean: " << graphs::stats::mean(data) << std::endl;
    std::cout << "Standard Deviation: " << graphs::stats::sd(data) << std::endl;
    
    // Error calculation (used for convergence checks)
    float sq_diff = graphs::stats::square_difference(data, target);
    std::cout << "Squared L2 Difference: " << sq_diff << std::endl;
}
```

## Graph Parsing (JSON)

Use the graphs::parser module to easily import/export graphs represented as adjacency maps in JSON strings.

JSON Format Example:

```json
{
  "0": [1, 2],
  "1": [3],
  "2": [0, 3],
  "3": []
}
```

Usage:

```c++
#include <graphs/parser/fromjson.hpp>

void load_graph() {
    std::string json_data = "{\"0\": [1, 2], \"1\": [3]}";
    
    MyGraph loaded_graph = graphs::parser::fromjson<MyGraph>(json_data);
    
    std::cout << "Graph loaded with " << loaded_graph.numVertices() << " vertices." << std::endl;
}
```
